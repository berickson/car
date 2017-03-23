
'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
import subprocess
import time
from flask import Flask, request, send_from_directory, render_template, jsonify

app = Flask(__name__)

@app.route('/')
def index():
    return send_from_directory('templates', 'index.html')

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
