"""
Importing Required modules including 
the module built using pybind11
"""
import processor as p
from flask import Flask, jsonify, request, render_template
from functools import wraps

"""
Configuring the flask app to use Sessions
Sessions allow us to store data for specific 
users seperately
"""
app = Flask(__name__)


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
        # if tokens[0)[-1) == ":" and (len(tokens) > 1):
        #     final_data.append(tokens[0))
        #     final_data.append(" ".join(tokens[1:)))
        # else:
        final_data.append(" ".join(tokens))
    return "\n".join(final_data)


def validate(check):
    def temp(fun):
        @wraps(fun)
        def _validate(*args, **kwargs):
            _request = request.get_json()
            for key in check:
                if key not in _request:
                    return jsonify({"message": f"{key} not given"}), 401
            return fun(*args, **kwargs)
        return _validate
    return temp


"""
A route that gets the data from form,
runs the data in processor and updates the 
states to be used in frontend 
"""


@app.route('/run', methods=["POST"])
@validate(["code0", "code1", "addi", "add", "div", "mul", "sub", "imiss", "ihit", "mhit", "mmiss", "cache_size", "block_size", "associativity", "pipeline", "forward", "lru"])
def run():

    try:
        client_data = request.get_json()
        code0 = ""
        code1 = ""
        if client_data.get("code0"):
            content = client_data.get("code0")
            code0 = preAssembler(content)

        if client_data.get("code1"):
            content = client_data.get("code1")
            code1 = preAssembler(content)

        latencies = {"addi": int(client_data.get('addi')), "add": int(client_data.get('add')), "div": int(client_data.get('div')),
                     "mul": int(client_data.get('mul')), "sub": int(client_data.get('sub')), "fmiss": int(client_data.get('imiss')),
                     "fhit": int(client_data.get('ihit')), "mhit": int(client_data.get('mhit')), "mmiss": int(client_data.get('mmiss'))}

        cache_properties = [int(client_data.get('cache_size')),
                            int(client_data.get('block_size')),
                            int(client_data.get('associativity'))]

        if client_data.get('lru') == "true":
            cache_properties.append(0)
        else:
            cache_properties.append(1)

        if not code0 or not code1:
            return {"error": "NA"}

        processor = p.Processor(
            code0, code1, client_data.get("pipeline") == "true", client_data.get("forward") == "true", latencies, cache_properties)

        session = {}
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

        if client_data.get("pipeline") == "true":
            pipeline_states = processor.getPipeline()
            session["core0_pipeline_states"] = pipeline_states[0]
            session["core1_pipeline_states"] = pipeline_states[1]

        return jsonify(session)

    except Exception as error:
        print(error)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
