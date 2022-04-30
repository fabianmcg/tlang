#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import pathlib
from subprocess import run as run_cmd
import time
import json


class RunConfiguration:
    def __init__(
        self,
        identifier: int = 0,
        machine: str = "",
        compiler: str = "",
        program: str = "",
        program_path: str = "",
        arguments: str = "",
        iteration_count: int = 0,
        extra_info: str = "",
    ):
        self.identifier = identifier
        self.machine = "" + machine
        self.compiler = "" + compiler
        self.program = "" + program
        self.arguments = "" + arguments
        self.program_path = program_path
        self.iteration_count = iteration_count
        self.extra_info = "" + extra_info

    def __repr__(self) -> str:
        return "\n".join(["  {:<20}{}".format("{}:".format(str(k).capitalize()), v) for k, v in self.__dict__.items()])

    def __str__(self) -> str:
        return repr(self)

    def __eq__(self, other):
        if isinstance(other, RunConfiguration):
            return self.to_dict() == other.to_dict()
        return False

    def to_dict(self):
        return self.__dict__

    @staticmethod
    def from_dict(confDict):
        conf = RunConfiguration()
        conf.__dict__.update(confDict)
        return conf

    def match(self, other, keys):
        if isinstance(other, RunConfiguration):
            sd = self.to_dict()
            od = other.to_dict()
            return [sd[key] for key in keys] == [od[key] for key in keys]
        return False

    def name(self):
        return "{}.{}.{}.{}.{}".format(self.program, self.compiler, self.machine, self.iteration_count, self.identifier)

    def command(self):
        return "{}{}".format(self.program_path, "" if len(self.arguments) == 0 else " {}".format(self.arguments))


class Run:
    def __init__(self, iteration, elapsed_time, out, err, status):
        self.iteration = iteration
        self.elapsed_time = elapsed_time
        self.stdout = out
        self.stderr = err
        self.status = status

    def __repr__(self) -> str:
        return str(self.__dict__)

    def __str__(self) -> str:
        return repr(self)

    def to_dict(self):
        return self.__dict__

    @staticmethod
    def from_dict(confDict):
        conf = RunConfiguration()
        conf.__dict__.update(confDict)
        return conf

    @staticmethod
    def run(configuration: RunConfiguration):
        runs = []
        successful = True
        command = configuration.command()
        for i in range(0, configuration.iteration_count):
            start = time.perf_counter_ns()
            retcode = run_cmd(command, shell=True, capture_output=True)
            stop = time.perf_counter_ns()
            runs.append(
                Run(
                    i,
                    stop - start,
                    retcode.stdout.decode("utf-8"),
                    retcode.stderr.decode("utf-8"),
                    int(retcode.returncode),
                )
            )
            print("Iteration: {}".format(i))
            print("  {:<20}{}".format("Elapsed:", (stop - start) * (1e-9)))
            print("  {:<20}{}".format("Status:", retcode.returncode))
            print("  {:<20}{}".format("Stdout:", retcode.stdout))
            print("  {:<20}{}".format("Stderr:", retcode.stderr))
            if retcode.returncode != 0:
                successful = False
                break
        return successful, runs


def parseArgs():
    parser = argparse.ArgumentParser(
        description="Run CMD",
        add_help=True,
        formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=60),
    )
    parser.add_argument(
        "-o",
        metavar="<output path>",
        type=str,
        default="-",
        help="Output filename",
    )
    parser.add_argument(
        "-i",
        metavar="<id>",
        type=int,
        default=0,
        help="Identifier",
    )
    parser.add_argument(
        "-m",
        metavar="<machine>",
        type=str,
        required=True,
        help="machine name",
    )
    parser.add_argument(
        "-c",
        metavar="<compiler>",
        type=str,
        required=True,
        help="compiler name",
    )
    parser.add_argument(
        "-p",
        metavar="<program>",
        type=str,
        required=True,
        help="program to run",
    )
    parser.add_argument(
        "-P",
        metavar="<program path>",
        type=str,
        default="",
        help="program path",
    )
    parser.add_argument(
        "-a",
        metavar="<program arguments>",
        type=str,
        default="",
        help="program arguments",
    )
    parser.add_argument(
        "-I",
        metavar="<iteration count>",
        type=int,
        default=1,
        help="iteration count",
    )
    parser.add_argument(
        "-E",
        metavar="<extra info>",
        type=str,
        default="",
        help="extra info to record",
    )
    args = parser.parse_args()
    if args.P == "":
        args.P = str(pathlib.Path(args.p).absolute())
        if not pathlib.Path(args.P).exists():
            raise RuntimeError("Program: {} doesn't exist".format(args.P))
    if args.I < 0:
        args.I = 0
    conf = RunConfiguration(args.i, args.m, args.c, args.p, args.P, args.a, args.I, args.E)
    if args.o == "-":
        args.o = "{}.json".format(conf.name())
    return args.o, conf


def main():
    output_name, configuration = parseArgs()
    print("Running:\n", configuration, sep="")
    start = time.perf_counter_ns()
    successful, results = Run.run(configuration)
    stop = time.perf_counter_ns()
    output = {
        "configuration": configuration.to_dict(),
        "elapsed_time": stop - start,
        "average_time": (stop - start) / len(results),
        "successful": successful,
        "runs": [run.to_dict() for run in results],
    }
    with open(output_name, "w") as outfile:
        json.dump(output, outfile, indent=2)
    print("Finished running:")
    print("  {:<20}{}".format("Elapsed time:", (stop - start) * (1e-9)))
    print("  {:<20}{}".format("Average time:", ((stop - start) / len(results)) * (1e-9)))
    print("  {:<20}{}".format("Status:", successful))
    print("  {:<20}{}".format("Iterations:", len(results)))

if __name__ == "__main__":
    main()
