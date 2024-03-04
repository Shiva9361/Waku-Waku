from flask import Flask, jsonify, request
from flask_cors import CORS
import os
from subprocess import Popen, PIPE

app = Flask(__name__)
"""
CORS is required to interact with vue.js
"""
CORS(app, resources={r"/*": {'origins': "*"}})

memory_dict = {}
core1_reg_states = []
core0_reg_states = []

core1_pipeline_states = []
core0_pipeline_states = []

core1_stats = []
core0_stats = []


@app.route('/memory')
def memory():
    global memory_dict
    return memory_dict


@app.route('/run', methods=["POST"])
def run():
    global memory_dict, core1_reg_states, core0_reg_states, core1_pipeline_states, core0_pipeline_states, core0_stats, core1_stats

    core1_reg_states = []
    core0_reg_states = []
    core1_pipeline_states = []
    core0_pipeline_states = []
    core0_stats = []
    core1_stats = []

    if request.method == "POST":
        """
        Remove the files of previous run
        """
        if not os.path.exists("a.out"):
            os.system("g++ main.cpp")
        if os.path.exists("data/core1_reg.txt"):
            os.remove("data/core1_reg.txt")
        if os.path.exists("data/core0_reg.txt"):
            os.remove("data/core0_reg.txt")
        if os.path.exists("data/core1_pipe.txt"):
            os.remove("data/core1_pipe.txt")
        if os.path.exists("data/core0_pipe.txt"):
            os.remove("data/core0_pipe.txt")

        instruction = []
        if request.form["pipeline"] == "true":
            if request.form["forward"] == "true":
                instruction = ["./a.out", "codes/selection_sort.s",
                               "codes/bubble_sort.s", "true", "true"]
            else:
                instruction = ["./a.out", "codes/selection_sort.s",
                               "codes/bubble_sort.s", "true", "false"]
        else:
            instruction = ["./a.out", "codes/selection_sort.s",
                           "codes/bubble_sort.s", "false", "false"]

        process = Popen(instruction, shell=False, stdout=PIPE, stdin=PIPE)

        input = f"{request.form['addi']} {request.form['add']} {request.form['div']} {request.form['mul']} {request.form['sub']}"

        stdout, stderr = process.communicate(input=str.encode(input))

        with open("data/memory_after.txt") as mem_file:
            memory = mem_file.read()
            memory.replace("\r\n", "\n")
            memory = memory.split("\n")
            memory_dict = {}
            for i in memory[:-1]:
                memory_dict[int(i.split(",")[0])] = int(i.split(",")[1])

        with open("data/core1_reg.txt") as core_reg_file:
            reg_states = core_reg_file.read()
            reg_states.replace("\r\n", "\n")
            reg_states = reg_states.split("\n")
            for state in reg_states:
                reg_dict = {}
                content = state.split("\t")
                for data in content:
                    if data != "":
                        reg, value = data.split(" ")
                        reg_dict[reg] = value

                core1_reg_states.append(reg_dict)
        with open("data/core0_reg.txt") as core_reg_file:
            reg_states = core_reg_file.read()
            reg_states.replace("\r\n", "\n")
            reg_states = reg_states.split("\n")
            for state in reg_states:
                reg_dict = {}
                content = state.split("\t")
                for data in content:
                    if data != "":
                        reg, value = data.split(" ")
                        reg_dict[reg] = value

                core0_reg_states.append(reg_dict)

        if request.form["pipeline"]:
            with open("data/core1_pipe.txt") as core_pipe_file:
                states = core_pipe_file.read().replace("\r\n", "\n").split("\n")
                template_state = ["write_back", "memory", "execute",
                                  "decode/register_fetch", "instruction_fetch"]
                for state in states:
                    state_list = state.split(" ")
                    if (len(state_list) > 5):
                        state_list = state_list[:-1]
                    for _ in range(len(state_list)):
                        if state_list[_] == "0":
                            state_list[_] = "stall"
                        else:
                            state_list[_] = template_state[_]
                    core1_pipeline_states.append(state_list)

            with open("data/core0_pipe.txt") as core_pipe_file:
                states = core_pipe_file.read().replace("\r\n", "\n").split("\n")
                template_state = ["write_back", "memory", "execute",
                                  "decode/register_fetch", "instruction_fetch"]
                for state in states:
                    state_list = state.split(" ")
                    if (len(state_list) > 5):
                        state_list = state_list[:-1]
                    for _ in range(len(state_list)):
                        if state_list[_] == "0":
                            state_list[_] = "stall"
                        else:
                            state_list[_] = template_state[_]
                    core0_pipeline_states.append(state_list)

            with open("data/stats.txt") as stats_file:
                stats = stats_file.read().replace("\r\n", "\n").split("\n")
                if stats[-1] == "":
                    stats = stats[:-1]
                core0_stats.extend([{"ic": stats[0]}, {"hc": stats[2]}, {"clk": stats[4]}, {
                                   "ipc": round(int(stats[0])/int(stats[4]), 3)}])
                core1_stats.extend([{"ic": stats[1]}, {"hc": stats[3]}, {"clk": stats[5]}, {
                                   "ipc": round(int(stats[1])/int(stats[5]), 3)}])

    return {"message": "Done"}


@app.route('/load', methods=["POST"])
def load():
    return None


@app.route('/core/0/reg')
def core0_reg():
    return jsonify(core0_reg_states)


@app.route('/core/1/reg')
def core1_reg():
    return jsonify(core1_reg_states)


@app.route('/core/0/pipe')
def core0_pipe():
    return jsonify(core0_pipeline_states)


@app.route('/core/1/pipe')
def core1_pipe():
    return jsonify(core1_pipeline_states)


@app.route('/core/0/stats')
def core0_stats_fun():
    return jsonify(core0_stats)


@app.route('/core/1/stats')
def core1_stats_fun():
    return jsonify(core1_stats)


if __name__ == "__main__":
    app.run(debug=True)
