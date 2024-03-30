from flask import Flask, jsonify, request, session, render_template
from flask_session import Session
import os
import processor as p
from datetime import timedelta

app = Flask(__name__)
app.secret_key = "not_so_secreta"
app.config["SESSION_TYPE"] = "filesystem"
app.config['PERMANENT_SESSION_LIFETIME'] = timedelta(minutes=5)

Session(app)


def clear_cores_reg():
    clear_reg_states = [
        {
            "0": "0",
            "1": "0",
            "10": "0",
            "11": "0",
            "12": "0",
            "13": "0",
            "14": "0",
            "15": "0",
            "16": "0",
            "17": "0",
            "18": "0",
            "19": "0",
            "2": "0",
            "20": "0",
            "21": "0",
            "22": "0",
            "23": "0",
            "24": "0",
            "25": "0",
            "26": "0",
            "27": "0",
            "28": "0",
            "29": "0",
            "3": "0",
            "30": "0",
            "31": "0",
            "4": "0",
            "5": "0",
            "6": "0",
            "7": "0",
            "8": "0",
            "9": "0"
        }
    ]
    return clear_reg_states


@app.route("/")
def index():
    return render_template("index.html")


@app.route('/mem')
def memory():
    if "memory" not in session:
        session["memory"] = [{str(i): 0 for i in range(1024)}]
    return session["memory"]


@app.route('/clear')
def clear():
    session.permanent = True
    session["core1_reg_states"] = clear_cores_reg()
    session["core0_reg_states"] = clear_cores_reg()
    session["core0_pipeline_states"] = []
    session["core1_pipeline_states"] = []
    session["core0_stats"] = {}
    session["core1_stats"] = {}
    session["memory"] = [{str(i): 0 for i in range(1024)}]
    return {"message": "done"}


@app.route('/run', methods=["POST"])
def run():

    if request.method == "POST":
        try:
            file0 = "codes/selection_sort.s"
            file1 = "codes/bubble_sort.s"
            if request.form["code0"]:
                content = request.form["code0"]
                content = content.replace("\r\n", "\n").replace(",", " ")
                with open("codes/slot0.s", "w") as slot0_file:
                    slot0_file.write(content)
                file0 = "codes/slot0.s"

            if request.form["code1"]:
                content = request.form["code1"]
                content = content.replace("\r\n", "\n").replace(",", " ")
                with open("codes/slot1.s", "w") as slot0_file:
                    slot0_file.write(content)
                file1 = "codes/slot1.s"

            latencies = {"addi": int(request.form['addi']), "add": int(request.form['add']),
                         "div": int(request.form['div']), "mul": int(request.form['mul']), "sub": int(request.form['sub']), "fmiss": 4, "fhit": 2}
            processor = p.Processor(
                file0, file1, request.form["pipeline"] == "true", request.form["forward"] == "true", latencies, [64, 8, 4, 0])
            clear()
            stats = processor.getStats()
            for _ in stats:
                for key in _:
                    if key == "IPC":
                        _[key] = round(_[key], 2)
                    else:
                        _[key] = int(_[key])

            session["core0_stats"] = stats[0]
            session["core1_stats"] = stats[1]
            session["memory"] = processor.getMemory()

            register_states = processor.getRegisters()
            session["core0_reg_states"] = register_states[0]
            session["core1_reg_states"] = register_states[1]

            if request.form["pipeline"] == "true":
                pipeline_states = processor.getPipeline()
                session["core0_pipeline_states"] = pipeline_states[0]
                session["core1_pipeline_states"] = pipeline_states[1]

        except Exception as error:
            print("hi")
            print(error)
            clear()

    return {"message": "Done"}


@app.route('/core/0/reg')
def core0_reg():
    if "core0_reg_states" not in session:
        session["core0_reg_states"] = clear_cores_reg()
    return jsonify(session["core0_reg_states"])


@app.route('/core/1/reg')
def core1_reg():
    if "core1_reg_states" not in session:
        session["core1_reg_states"] = clear_cores_reg()
    return jsonify(session["core1_reg_states"])


@app.route('/core/0/pipe')
def core0_pipe():
    if "core0_pipeline_states" not in session:
        session["core0_pipeline_states"] = []
    return jsonify(session["core0_pipeline_states"])


@app.route('/core/1/pipe')
def core1_pipe():
    if "core1_pipeline_states" not in session:
        session["core1_pipeline_states"] = []
    return jsonify(session["core1_pipeline_states"])


@app.route('/core/0/stats')
def core0_stats_fun():
    if "core0_stats" not in session:
        session["core0_stats"] = []
    return jsonify(session["core0_stats"])


@app.route('/core/1/stats')
def core1_stats_fun():
    if "core1_stats" not in session:
        session["core0_stats"] = []
    return jsonify(session["core1_stats"])


if __name__ == "__main__":
    app.run(debug=True)
