from flask import Flask,jsonify
from flask_cors import CORS
import os
from subprocess import Popen, PIPE

app = Flask(__name__)
"""
CORS is required to interact with vue.js
"""
CORS(app,resources={r"/*":{'origins':"*"}})

memory_dict = {}
core1_reg_states = []
core0_reg_states = []

@app.route('/memory')
def memory():
    global memory_dict
    return memory_dict
@app.route('/run')
def run():
    global memory_dict,core1_reg_states,core0_reg_states
    core1_reg_states = []
    core0_reg_states = []

    if not os.path.exists("a.out"):
        os.system("g++ main.cpp")
    if os.path.exists("core1.txt"):
        os.remove("core1.txt")
    if os.path.exists("core0.txt"):
        os.remove("core0.txt")

    process = Popen(['./a.out',"codes/selection_sort.s","codes/selection_sort.s"], shell=False, stdout= PIPE, stdin=PIPE)
    stdout, stderr = process.communicate()

    with open("memory_after.txt") as mem_file:
        memory= mem_file.read()
        memory.replace("\r\n","\n")
        memory = memory.split("\n")
        memory_dict = {}
        for i in memory[:-1]:
            memory_dict[int(i.split(",")[0])] = int(i.split(",")[1])

    with open("core1.txt") as core_reg_file:
        reg_states= core_reg_file.read()
        reg_states.replace("\r\n","\n")
        reg_states = reg_states.split("\n")
        for state in reg_states:
            reg_dict = {}
            content = state.split("\t")
            for data in content:
                if data !="":
                    reg,value = data.split(" ")
                    reg_dict[reg] = value
            
            core1_reg_states.append(reg_dict)
    with open("core0.txt") as core_reg_file:
        reg_states= core_reg_file.read()
        reg_states.replace("\r\n","\n")
        reg_states = reg_states.split("\n")
        for state in reg_states:
            reg_dict = {}
            content = state.split("\t")
            for data in content:
                if data !="":
                    reg,value = data.split(" ")
                    reg_dict[reg] = value
            
            core0_reg_states.append(reg_dict)

    return {"message":"Done"}
@app.route('/load',methods = ["POST"])
def load():
    return None

@app.route('/core/0/reg')
def core0_reg():
    return jsonify(core0_reg_states)

@app.route('/core/1/reg')
def core1_reg():
    return jsonify(core1_reg_states)
if __name__ == "__main__":
    app.run(debug=True)
    

    