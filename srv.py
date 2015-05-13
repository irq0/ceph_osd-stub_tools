import os
from flask import Flask, request, redirect, url_for
from werkzeug import secure_filename

UPLOAD_FOLDER = '/tmp'

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


@app.route('/objects/<path:oid>', methods=['PUT'])
def upload(oid):
    fn = os.path.join(app.config["UPLOAD_FOLDER"], secure_filename(oid))
    with open(fn, "w") as f:
        d = request.stream.read()
        f.write(d)
        app.logger.debug("saved obj \"{}\" to {}".format(oid, fn))

        return fn


@app.route("/objects/<path:oid>", methods=["GET"])
def download(oid):
    fn = os.path.join(app.config["UPLOAD_FOLDER"], secure_filename(oid))
    with open(fn, "r") as f:
        app.logger.debug("retrieved obj \"{}\" from {}".format(oid, fn))

        return f.read()


@app.route("/objects/<path:oid>", methods=["DELETE"])
def delete(oid):
    fn = os.path.join(app.config["UPLOAD_FOLDER"], secure_filename(oid))

    app.logger.debug("removing obj \"{}\" file {}".format(oid, fn))

    os.remove(fn)
    return fn

if __name__ == '__main__':
    app.debug = True
    app.run()
