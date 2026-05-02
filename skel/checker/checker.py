#!/usr/bin/env python3

import subprocess
import os
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
from threading import Semaphore # for stdout synchronization

from typing import List, Dict

cwd = '..' if os.getcwd().endswith('checker') else '.'
EXE_PATH = [f"{cwd}/checker/", f"{cwd}/checker/build/"]
EXE = "main"
TESTS_DIRECTORY = f"{cwd}/checker/tests/"
T_COL_IN = lambda x, y: (f"in/T{x}_test{y}.in", f"out/T{x}_test{y}.out", f"ref/T{x}_test{y}.ref")
T_NO = {1: 10, 2: 10, 3: 10, 4: 10, 5: 8, 6: 10}
T_VALUE = {1: 10, 2: 30, 3: 15, 4: 15, 5: 20, 6: 20}

executable = None
score: float = 0.0

semaphores = [Semaphore(0) for _ in range(6)]
semaphores[0].release()  # allow the first task to print immediately
outputs = [[] for _ in range(6)]

def run_test(test_no: int = 0, task_no: int = 0):
    global score
    if task_no == 0 or test_no == 0 or task_no > 6 or test_no > 10:
        return 'Invalid task or test number'

    if not executable:
        raise Exception('Executable not found')

    ifl, ofl, rfl = T_COL_IN(task_no, test_no)
    error = None
    result = subprocess.run([f"./{executable}"], stdin=open(f"{TESTS_DIRECTORY}{ifl}", "r"), stdout=open(f"{TESTS_DIRECTORY}{ofl}", "w"))
    if result.returncode != 0:
        error = "Failed | return code not 0: 0.0"

    result = subprocess.run(["diff", f"{TESTS_DIRECTORY}{ofl}", f"{TESTS_DIRECTORY}{rfl}"], stdout=open(os.devnull, "w"), stderr=subprocess.PIPE)
    if result.returncode != 0:
        error = "Failed | output mismatch: 0.0"

    output = f"Running test {test_no} for task {task_no}:"
    if error:
        output += f" {error}"
    else:
        output += f" Passed: {T_VALUE[task_no] / T_NO[task_no]}"
        score += T_VALUE[task_no] / T_NO[task_no]

    # Store the output in a list
    outputs[task_no - 1].append(output)

    # Release the semaphore for the next task
    if task_no < 6:
        semaphores[task_no].release()
    
n_cpu = os.cpu_count()
thpool: ThreadPoolExecutor = ThreadPoolExecutor(max_workers=n_cpu)

def run_task(task_no: int = 0):
    if task_no not in T_NO:
        return 'Invalid task number'

    for test_no in range(1, T_NO[task_no] + 1):
        run_test(test_no, task_no)

def run_all():
    futures = []
    for task_no in range(1, 7):
        futures.append(thpool.submit(run_task, task_no))
    
    for future in as_completed(futures):
        future.result()
    
    thpool.shutdown()

stdout_lock = threading.Lock()
def check_if_executable_exists():
    # check if the root directory contains an elf executable
    global executable
    # default executable path
    executable = f"./main"
    for path in EXE_PATH:
        files = os.listdir(path)
        print(f"files @ {path}: {files}")
        # check if filetype is ELF
        # run "file" command in shell and check for "ELF" in the output
        for file in files:
            details = subprocess.run(["file", path + file], capture_output=True).stdout.decode()
            if "ELF" in details and "GNU/Linux" in details and "linked" in details:
                executable = path + file
                print(f"Executable found: {executable}")
                break


def main():
    global score
    subprocess.run(["make"])
    check_if_executable_exists()

    run_all()

    # Print the outputs in order
    for i in range(6):
        semaphores[i].acquire()
        for output in outputs[i]:
            print(output)
        if i < 5:
            semaphores[i + 1].release()

    print(f"Total (tests): {score}")
if __name__ == "__main__":
    main()


