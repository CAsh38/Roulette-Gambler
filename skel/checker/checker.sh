#!/bin/bash

total_score=0

function init {
    # remove already existing source files
    mkdir -p build
    rm -rf build/*

    rsync -av --ignore-existing ../src/ .
}

# Function to run the Python checker script
function run_python_checker {
    echo "Running Python checker..."
    output=$(python3 checker.py)
    echo "$output"
    last_line=$(echo "$output" | tail -n 1)
    total_points=$(echo "$last_line" | awk '{print $NF}')
    # total_points=$(printf "%.0f" "$total_points_match")
    # echo "Total points: $total_points"
    total_points_int=${total_points%.*}
    total_score=$((total_score+total_points_int))
    if [ "$total_points_int" -ge 90 ]; then
        return 0
    else
        return 1
    fi
}

# Function to run Valgrind
function run_valgrind {
    echo "Running Valgrind..."
    # for each input test, run valgrind, and if there are no memory leaks, increment a variable
    # if the variable is equal to the number of tests, then all tests passed
    tests_passed=0
    # for file in tests/in/
    for file in tests/in/*
    do
        # echo "Running valgrind on $file"
        valgrind --leak-check=full --error-exitcode=1 --log-file=valgrind.log ./main < $file > /dev/null &>> result
        if [ $? -eq 0 ]; then
            tests_passed=$((tests_passed+1))
        fi
    done
    echo "Valgrind tests passed: $tests_passed"
    if [ $tests_passed -gt 47 ]; then
        echo "Valgrind passed. +5"
        total_score=$((total_score+5))
    else
        echo "Valgrind failed. +0"
    fi
}

# Function to check coding style using cpplint
function check_coding_style {
    echo "Checking coding style with cpplint..."
    cpplint --filter=-legal/copyright,-readability/casting,-build/include_subdir,-runtime/threadsafe_fn,-build/header_guard,-runtime/int \
        --linelength=120 ../src/*.c &>> result

    if [ $? -eq 0 ]; then
        echo "Coding style check passed. +5"
        total_score=$((total_score+5))
    else
        echo "Coding style check failed. +0"
    fi
}

function check_readme {
    ls . | grep -i "readme" &>/dev/null
    if [ $? -eq 0 ]; then
        echo "README file found. +0"
    else
        echo "README file not found. -5"
        total_score=$((total_score-5))
    fi
}

init
# Main script execution
run_python_checker
if [ $? -eq 0 ]; then
    run_valgrind
else
    echo "Not all tests passed @ tasks [1..5]. Skipping Valgrind."
fi

check_coding_style
check_readme

echo " "
echo " "
printf "Total: ${total_score}/100\n"
