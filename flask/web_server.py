# -*- coding: utf-8 -*-

'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
import pandas as pd # must import BEFORE flask or high CPU on PI
from flask import Flask, request, send_from_directory, jsonify, json, Response
import socket
import tracks
import psutil
import os

TRACK_STORAGE = tracks.TrackStorage()

app = Flask(__name__, static_folder='static', static_url_path='')

#@app.route('/angular.min.js')
#@app.route('/sitemap.xml')
#def static_from_root():
#    return send_from_directory(app.static_folder, request.path[1:])

@app.route('/')
def index():
    return send_from_directory('static', 'index.html')



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
        return jsonify(error='error reading cpu_percent')

@app.route('/car/get_state')
def get_car_state():
#    recv_string = '{"vbat":3}'
    try:
        connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connection.connect(('localhost', 5571))
        connection.send(("get_state\x00").encode())
        recv_string = connection.recv(1000).decode("utf-8").rstrip('\0')
        connection.close()
    except:
        pass
    return Response(recv_string,mimetype='application/json')


@app.route('/command/reset_odometer', methods=['PUT'])
def command_reset_odometer():
    try:
        connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connection.connect(('localhost', 5571))
        connection.send(("reset_odometer\x00").encode())
        recv_string = connection.recv(1000).decode("utf-8").rstrip('\0')
        connection.close()
    except:
        pass
    return jsonify(result=recv_string)


@app.route('/command/go', methods=['PUT'])
def command_go():
#    recv_string = '{"vbat":3}'
    try:
        connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connection.connect(('localhost', 5571))
        connection.send(("go\x00").encode())
        recv_string = connection.recv(1000).decode("utf-8").rstrip('\0')
        connection.close()
    except:
        pass
    return jsonify(result=recv_string)

@app.route('/command/stop', methods=['PUT'])
def command_stop():
    try:
        connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connection.connect(('localhost', 5571))
        connection.send(("stop\x00").encode())
        recv_string = connection.recv(1000).decode("utf-8").rstrip('\0')
        connection.close()
    except:
        pass
    return jsonify(result=recv_string)

@app.route('/command/record', methods=['PUT'])
def command_record():
    try:
        connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connection.connect(('localhost', 5571))
        connection.send(("record\x00").encode())
        recv_string = connection.recv(1000).decode("utf-8").rstrip('\0')
        connection.close()
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

@app.route('/tracks/<track_name>')
def get_track_options(track_name):
    return jsonify(track={'name', track_name})

@app.route('/tracks/<track_name>/route_names')
def get_route_names(track_name):
    routes = TRACK_STORAGE.get_track(track_name).get_routes()
    return jsonify(route_names=[route.get_name() for route in routes])

@app.route('/tracks/<track_name>/routes')
def get_routes(track_name):
    routes = TRACK_STORAGE.get_track(track_name).get_routes()
    return jsonify(routes=[{'name':route.get_name()} for route in routes])

@app.route('/tracks/<track_name>/routes/<route_name>')
def get_route(track_name, route_name):
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    return jsonify({'name', route.get_name()})

@app.route('/tracks/<track_name>/routes/<route_name>/path', methods=['GET','PUT'])
def get_path(track_name, route_name):
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    path_path = route.folder+"/path.csv"
    # allowed value are {‘split’,’records’,’index’,’columns’,’values’}
    orient = request.args.get('orient', 'records')
    if request.method == 'GET':
        df = pd.read_csv(path_path)
        s = df.to_json(orient=orient)
        return s
    elif request.method == 'PUT':
        app.logger.error("saving path")
        route = json.dumps(request.json)
        app.logger.error("route" + route)
        df = pd.read_json(route)
        df.to_csv(
            path_path,
            index = False,
            columns=["secs","x","y","rear_x", "rear_y", "reverse", "heading","adj","esc","str","m/s","road_sign_label","road_sign_command","arg1","arg2","arg3"])
        return "ok"

#@app.route('/run_settings', methods=['PUT'])
#def put_run_settings():
##    return
    #return Response("ok")
#    try:
#    except Exception:
#        return "I'm having a hard time"
#        return Response("there was an error writing run settings " + request)

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

@app.route('/car/status')
def car_status():
    car_state = {
        'esc':1500,
        'str':1700,
        'us':1234343,
        'name':'Erickson, Brian',
        'description': 'he said "it would work".'
    }
    return jsonify(car_state)



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
