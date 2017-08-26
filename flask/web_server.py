# -*- coding: utf-8 -*-

'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
import pandas as pd # must import BEFORE flask or high CPU on PI
from flask import Flask, request, send_from_directory, jsonify,Response
import socket
import tracks
import psutil

TRACK_STORAGE = tracks.TrackStorage()

app = Flask(__name__)

@app.route('/')
def index():
    return send_from_directory('templates', 'index.html')


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


@app.route('/d3')
def get_d3():
    return send_from_directory('templates', 'd3.html')

@app.route('/tracks')
def get_tracks():
    tracks = TRACK_STORAGE.get_tracks()
    return jsonify(tracks=[{'name': t.get_name()} for t in tracks])

@app.route('/tracks/<track_name>')
def get_track_options(track_name):
    return jsonify(track={'name', track_name})

@app.route('/tracks/<track_name>/routes')
def get_routes(track_name):
    routes = TRACK_STORAGE.get_track(track_name).get_routes()
    return jsonify(routes=[{'name':route.get_name()} for route in routes])

@app.route('/tracks/<track_name>/routes/<route_name>')
def get_route(track_name, route_name):
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    return jsonify({'name', route.get_name()})

@app.route('/tracks/<track_name>/routes/<route_name>/path')
def get_path(track_name, route_name):
    # allowed value are {‘split’,’records’,’index’,’columns’,’values’}
    orient = request.args.get('orient', 'records')
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    path_path = route.folder+"/path.csv"
    df = pd.read_csv(path_path)
    s = df.to_json(orient=orient)
    return s


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
