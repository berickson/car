
'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
import subprocess
import time
from flask import Flask, request, send_from_directory, render_template, jsonify
import tracks
import pandas as pd

TRACK_STORAGE = tracks.TrackStorage()

app = Flask(__name__)

@app.route('/')
def index():
    return send_from_directory('templates', 'index.html')

@app.route('/tracks')
def get_tracks():
    tracks = TRACK_STORAGE.get_tracks()
    return jsonify([t.get_name() for t in tracks])

@app.route('/tracks/<track_name>')
def get_track_options(track_name):
    return jsonify(['routes'])

@app.route('/tracks/<track_name>/routes')
def get_routes(track_name):
    routes = TRACK_STORAGE.get_track(track_name).get_routes()
    return jsonify([route.get_name() for route in routes])

@app.route('/tracks/<track_name>/routes/<route_name>')
def get_route(track_name, route_name):
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    return jsonify(['runs'])

@app.route('/tracks/<track_name>/routes/<route_name>/path')
def get_path(track_name, route_name):
    # allowed value are {‘split’,’records’,’index’,’columns’,’values’}
    orient = request.args.get('orient', 'records')
    route = TRACK_STORAGE.get_track(track_name).get_route(route_name)
    path = route.folder+"/path.csv"
    return pd.read_csv(path).to_json(orient=orient)

@app.route('/ps')
def ps():
    cmd = "ps -eo pid,pmem,pcpu,start,time,comm --sort -c"
    ps_lines = subprocess.getoutput(cmd).split('\n')[:30]
    return jsonify(ps_lines)




def csv_val(s):
    s = str(s)

    s = s.replace('"', '""')
    if "," in s:
        s = '"'+s+'"'
    return s

def csv_line(items):
    '''
    returns items foramatted as a line in a csv per rfc4180
    without a trailing carriage return
    '''
    return ",".join([csv_val(item) for item in items])

@app.route('/car/status')
def car_status():
    car_state = {
        'esc':1500,
        'str':1700,
        'us':1234343,
        'name':'Erickson, Brian',
        'description': 'he said "it would work".'
    }
    all_fields = list(car_state.keys())
    fields = request.args.get('fields', default=None)
    fields = all_fields if fields is None else fields.split(',')
    values = []
    for field in fields:
        if field in car_state.keys():
            values.append(car_state[field])
        else:
            values.append("")

    rv = ""
    if request.args.get('header', default=False):
        rv += ",".join(fields)+"\n"
    rv += csv_line(values)+"\n"
    return rv


@app.route('/<path>')
def send_js(path):
    return send_from_directory('content', path)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
