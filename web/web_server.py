# -*- coding: utf-8 -*-

'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
import pandas as pd # must import BEFORE flask or high CPU on PI
from flask import Flask, request, send_from_directory, jsonify, json, Response, abort, render_template
import socket
import tracks
import psutil
import os
import time

TRACK_STORAGE = tracks.TrackStorage()

app = Flask(__name__, static_folder='static', static_url_path='')

#@app.route('/angular.min.js')
#@app.route('/sitemap.xml')
#def static_from_root():
#    return send_from_directory(app.static_folder, request.path[1:])


def robot_vars():
    robot = os.environ["ROBOT"]
    if robot=="blue":
        return {
            "name" : "Blue Crash",
            "color" : "blue"
        }
    if robot=="orange":
        return {
            "name" : "Orange Crash",
            "color" : "#FFB97D"
        }
    return {
        "name" : "Orange Crash",
        "color" : "#555555"
    }

@app.route('/')
def index():
    return render_template('index.html', robot=robot_vars())



class CommandError(Exception):
    status_code = 400

    def __init__(self, message, status_code=None, payload=None):
        Exception.__init__(self)
        self.message = message
        if status_code is not None:
            self.status_code = status_code
        self.payload = payload

    def to_dict(self):
        rv = dict(self.payload or ())
        rv['message'] = self.message
        return rv

@app.route('/pi/poweroff', methods=['PUT'])
def pi_poweroff():
    os.system("sudo poweroff")


@app.route('/pi/get_state')
def get_pi_state():
    try:
        cpu = psutil.cpu_percent(percpu=True)
        return jsonify(cpu=cpu)
    except:
        abort(500, 'error reading cpu_percent')

# implements constructor / destructor semantics to help socket lifetime
class get_car_socket:
    def __enter__(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
            self.socket.settimeout(5);
            self.socket.connect(('localhost', 5571));
            return self
        except:
            pass
        abort(500, 'error getting car socket')

    def __exit__(self, type, value, traceback):
        try:
            self.socket.close();
        except:
            pass

    def send(self, s):
        try:
            self.socket.send((s+"\x00").encode());
        except:
            abort(500, 'error sending to car socket')

    def receive(self):
        try:
            rv = ""
            done = False
            while not done:
                s = self.socket.recv(4096).decode("utf-8")
                done = s.endswith('\0')
                rv = rv + s.rstrip('\0')
            return rv
        except:
            pass
        abort(500, 'error reading car socket')

@app.route('/car/get_state')
def get_car_state():
    recv_string = "error"
    with get_car_socket() as socket:
        socket.send("get_state");
        recv_string = socket.receive()
    return Response(recv_string,mimetype='application/json')

@app.route('/car/get_scan')
def get_car_scan():
#    recv_string = '{"vbat":3}'
    rv = ""
    try:
        since = request.args.get('since', '-1')
        with get_car_socket() as socket:
            socket.send("get_scan,"+str(since))
            rv = socket.receive()
    except:
        pass
    return Response(rv,mimetype='application/json')    


@app.route('/command/reset_odometer', methods=['PUT'])
def command_reset_odometer():
    recv_string = "error"
    try:
        with get_car_socket() as socket:
            socket.send("reset_odometer")
            recv_string = socket.receive()
    except:
        pass
    return jsonify(result=recv_string)


@app.route('/command/go', methods=['PUT'])
def command_go():
    recv_string = "error"
    try:
        with get_car_socket() as socket:
            socket.send("go")
            recv_string = socket.receive()
    except:
        pass
    return jsonify(result=recv_string)

@app.route('/command/stop', methods=['PUT'])
def command_stop():
    try:
        with get_car_socket() as socket:
            socket.send("stop")
            recv_string = socket.receive()
    except:
        pass
    return jsonify(result=recv_string)

@app.route('/command/record', methods=['PUT'])
def command_record():
    try:
        with get_car_socket() as socket:
            socket.send("record")
            recv_string = socket.receive()
    except:
        pass
    return jsonify(result=recv_string)


@app.route('/d3')
def get_d3():
    return send_from_directory('templates', 'd3.html')

@app.route('/track_names')
def get_track_names():
    tracks = TRACK_STORAGE.get_tracks()
    return jsonify(track_names=[t.get_name() for t in tracks])

@app.route('/tracks')
def get_tracks():
    tracks = TRACK_STORAGE.get_tracks()
    return jsonify(tracks=[{'name': t.get_name()} for t in tracks])

@app.route('/tracks/<track_name>', methods=['GET','PUT', 'DELETE'])
def get_track_options(track_name):
    if request.method == 'GET':
        return jsonify(track={'name': track_name})
    elif request.method == 'PUT':
        TRACK_STORAGE.add_track(track_name)
        return jsonify(track={'name': track_name})
    elif request.method == 'DELETE':
        pass

@app.route('/tracks/<track_name>/route_names')
def get_route_names(track_name):
    routes = TRACK_STORAGE.get_track(track_name).get_routes()
    return jsonify(route_names=[route.get_name() for route in routes])

@app.route('/tracks/<track_name>/routes')
def get_routes(track_name):
    routes = TRACK_STORAGE.get_track(track_name).get_routes()
    return jsonify(routes=[{
        'name':route.get_name(),
        'time':time.strftime('%Y-%m-%dT%H:%M:%SZ',
        route.get_time())
        } for route in routes])

@app.route('/tracks/<track_name>/routes/<route_name>/runs')
def get_runs(track_name, route_name):
    runs = TRACK_STORAGE.get_track(track_name).get_route(route_name).get_runs()
    # return "found runs - " + str(len(runs))
    return jsonify(runs=[{'name':run.get_name(),'time':time.strftime('%Y-%m-%dT%H:%M:%SZ',run.get_time())} for run in runs])


@app.route('/tracks/<track_name>/routes/<route_name>')
def get_route(track_name, route_name):
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    return jsonify(name=route.get_name(),time=time.strftime('%Y-%m-%dT%H:%M:%SZ',route.get_time()))



@app.route('/tracks/<track_name>/routes/<route_name>/runs/<run_name>/path')
def get_run_path(track_name, route_name, run_name):
    run = TRACK_STORAGE.get_track(track_name).get_route(route_name).get_run(run_name);
    path_path = run.folder+"/path.csv"
    # allowed value are {‘split’,’records’,’index’,’columns’,’values’}
    orient = request.args.get('orient', 'records')
    df = pd.read_csv(path_path) if os.path.exists(path_path) else pd.DataFrame()
    s = df.to_json(orient=orient)
    return s


@app.route('/tracks/<track_name>/routes/<route_name>/path', methods=['GET','PUT'])
def get_path(track_name, route_name):
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    path_path = route.folder+"/path.csv"
    # allowed value are {‘split’,’records’,’index’,’columns’,’values’}
    orient = request.args.get('orient', 'records')
    if request.method == 'GET':
        df = pd.read_csv(path_path) if os.path.exists(path_path) else pd.DataFrame()
        s = df.to_json(orient=orient)
        return s
    elif request.method == 'PUT':
        #app.logger.error("saving path")
        route = json.dumps(request.json)
        #app.logger.error("route" + route)
        df = pd.read_json(route)
        df.to_csv(
            path_path,
            index = False,
            columns=[
                "secs",
                "x",
                "y",
                "rear_x",
                "rear_y",
                "reverse",
                "heading",
                "adj",
                "esc",
                "str",
                "m/s",
                "road_sign_label",
                "road_sign_command",
                "arg1",
                "arg2",
                "arg3"])

        return "ok"



@app.route('/run_settings', methods=['GET','PUT'])
def run_settings():
    path = tracks.get_run_settings_path()
    if request.method == 'PUT':
        run_settings = request.json
        with open(path, 'w+') as f:
            f.write(json.dumps(request.json))
            #f.write(request.data)
        return "ok"
    elif request.method == 'GET':
        path = 'empty'
        try:
            path = tracks.get_run_settings_path()
            with open(path, 'r') as f:
                run_settings = f.read()
        except Exception:
            return "there was an error reading " + path
        return Response(run_settings, mimetype='application/json')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
