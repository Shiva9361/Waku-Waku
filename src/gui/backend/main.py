"""
Importing Required modules including 
the module built using pybind11
"""
import processor as p
from flask import Flask, jsonify, request, session, render_template
from flask_session import Session
from flask_cors import CORS
from datetime import timedelta

"""
Configuring the flask app to use Sessions
Sessions allow us to store data for specific 
users seperately
"""
app = Flask(__name__)
app.secret_key = "not_so_secreta"
app.config["SESSION_TYPE"] = "filesystem"
app.config['PERMANENT_SESSION_LIFETIME'] = timedelta(minutes=5)
CORS(app, origins="*")
Session(app)


"""
A function to parse the form data sent by 
Frontend as this parsing is significantly 
more difficult in CPP
"""


def preAssembler(data):
    final_data = []
    data = data.replace("\r\n", "\n").split("\n")
    for line in data:
        formatted_line = line.lstrip().replace(
            ",", " ").replace("\r\n", "\n").rstrip()
        tokens = formatted_line.split(" ")
        # if tokens[0][-1] == ":" and (len(tokens) > 1):
        #     final_data.append(tokens[0])
        #     final_data.append(" ".join(tokens[1:]))
        # else:
        final_data.append(" ".join(tokens))
    return "\n".join(final_data)


"""
The default route of the website
"""


@app.route("/")
def index():
    return render_template("index.html")


"""
A route to load instructions and data into memory
"""


@app.route("/initialmem")
def initial_mem():
    if "initmem" not in session:
        session["initmem"] = {str(i): 0 for i in range(1024)}
    return session["initmem"]


"""
A route that returns changes to be done to memory
and at which cycle the change must be done 
"""


@app.route('/mem')
def memory():
    if "memory" not in session:
        session["memory"] = []
    return session["memory"]


"""
A route that returns changes to be done to cache
and at which cycle the change must be done 
"""


@app.route('/cache')
def cache():
    if "cache" not in session:
        session["cache"] = []
    return session["cache"]


@app.route('/cachestats')
def cache_stats():
    if "cache_stats" not in session:
        session["cache_stats"] = []
    return session["cache_stats"]


"""
A route that clears all data in session 
"""


@app.route('/clear')
def clear():
    session.permanent = True
    """
    Poping all previous data
    """
    pop_data = ["core0_pipeline_state", "core1_pipeline_state",
                "core1_reg_states", "core0_reg_states", "core0_stats",
                "core1_stats", "memory", "cache_stats"]
    for _ in pop_data:
        if _ in session:
            session.pop(_)
    session["initmem"] = {str(i): 0 for i in range(1024)}

    return {"message": "done"}


"""
A route that gets the data from form,
runs the data in processor and updates the 
states to be used in frontend 
"""


@app.route('/run', methods=["POST"])
def run():

    if request.method == "POST":
        try:
            code0 = ""
            code1 = ""
            if request.form["code0"]:
                content = request.form["code0"]
                code0 = preAssembler(content)

            if request.form["code1"]:
                content = request.form["code1"]
                code1 = preAssembler(content)

            latencies = {"addi": int(request.form['addi']), "add": int(request.form['add']), "div": int(request.form['div']),
                         "mul": int(request.form['mul']), "sub": int(request.form['sub']), "fmiss": int(request.form['imiss']),
                         "fhit": int(request.form['ihit']), "mhit": int(request.form['mhit']), "mmiss": int(request.form['mmiss'])}

            cache_properties = [int(request.form['cache_size']),
                                int(request.form['block_size']),
                                int(request.form['associativity'])]

            if request.form['lru'] == "true":
                cache_properties.append(0)
            else:
                cache_properties.append(1)

            if not code0 or not code1:
                return {"error": "NA"}

            processor = p.Processor(
                code0, code1, request.form["pipeline"] == "true", request.form["forward"] == "true", latencies, cache_properties)
            clear()
            stats = processor.getStats()
            for _ in stats:
                for key in _:
                    if key == "IPC":
                        _[key] = round(_[key], 2)
                    else:
                        _[key] = int(_[key])
            initial_mem = processor.getInitialMemory()
            for _ in range(1024):
                if (_ not in initial_mem):
                    initial_mem[_] = 0
            session["initmem"] = initial_mem
            session["core0_stats"] = stats[0]
            session["core1_stats"] = stats[1]

            cache_stats = {}
            cache_stats["hit-rate-0"] = round(stats[2]["hits0"] /
                                              (stats[2]["hits0"]+stats[2]["misses0"]) * 100, 2) if (stats[2]["hits0"]+stats[2]["misses0"]) > 0 else 0
            cache_stats["hit-rate-1"] = round(stats[2]["hits1"] /
                                              (stats[2]["hits1"]+stats[2]["misses1"]) * 100, 2) if (stats[2]["hits1"]+stats[2]["misses1"]) > 0 else 0
            cache_stats["miss-rate-0"] = round(stats[2]["misses0"] /
                                               (stats[2]["hits0"]+stats[2]["misses0"]) * 100, 2) if (stats[2]["hits0"]+stats[2]["misses0"]) > 0 else 0
            cache_stats["miss-rate-1"] = round(stats[2]["misses1"] /
                                               (stats[2]["hits1"]+stats[2]["misses1"]) * 100, 2) if (stats[2]["hits1"]+stats[2]["misses1"]) > 0 else 0
            session["cache_stats"] = cache_stats

            session["cache"] = processor.getCache()
            session["memory"] = processor.getMemory()

            register_states = processor.getRegisters()
            session["core0_reg_states"] = register_states[0]
            session["core1_reg_states"] = register_states[1]

            if request.form["pipeline"] == "true":
                pipeline_states = processor.getPipeline()
                session["core0_pipeline_states"] = pipeline_states[0]
                session["core1_pipeline_states"] = pipeline_states[1]
        except Exception as error:
            print(error)
            clear()

    return {"message": "Done"}


"""
Routes to get the incremental register states
of both cores 
"""


@app.route('/core/0/reg')
def core0_reg():
    if "core0_reg_states" not in session:
        session["core0_reg_states"] = {}
    return jsonify(session["core0_reg_states"])


@app.route('/core/1/reg')
def core1_reg():
    if "core1_reg_states" not in session:
        session["core1_reg_states"] = {}
    return jsonify(session["core1_reg_states"])


"""
Routes to get the pipeline states
of both cores 
"""


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


"""
Routes to get the stats
of both cores 
"""


@app.route('/core/0/stats')
def core0_stats_fun():
    if "core0_stats" not in session:
        session["core0_stats"] = []
    return jsonify(session["core0_stats"])


@app.route('/core/1/stats')
def core1_stats_fun():
    if "core1_stats" not in session:
        session["core1_stats"] = []
    return jsonify(session["core1_stats"])


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
