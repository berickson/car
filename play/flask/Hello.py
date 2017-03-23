'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
from flask import Flask, request, send_from_directory
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello World\n'


def csv_val(s):
    s = str(s)

    s = s.replace('"','""')
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
    try:
        car_state = {
            'esc':1500,
            'str':1700,
            'us':1234343,
            'name':'Erickson, Brian',
            'description': 'he said "it would work".'
        }
        all_fields = list(car_state.keys())
        fields = request.args.get('fields',default=None)
        if fields is None:
            fields = all_fields
        else:
            fields = fields.split(',')
    except:
        e = sys.exc_info()[0]
        return(str(e))
        values = [car_state[field] for field in fields]
    return csv_line(values)


@app.route('/<path>')
def send_js(path):
    return send_from_directory('content', path)

if __name__ == '__main__':
    app.run()


