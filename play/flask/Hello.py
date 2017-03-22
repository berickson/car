'''
started from tutorial at https://www.tutorialspoint.com/flask/flask_application.htm
'''
from flask import Flask, request, send_from_directory
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello World'
@app.route('/<path>')
def send_js(path):
    print('trying to server', path)
    return send_from_directory('content', path)

if __name__ == '__main__':
    app.run()


