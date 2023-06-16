from waitress import serve
from flask import Flask, request
import json
from threading import Thread
from subprocess import getoutput as go
import subprocess

app = Flask(__name__)

def runExe():
	result = subprocess.run(["./build/speedEstimation_v3", "/media/sd/speedEstimationFiles/AZMES_ME06EKIN_2023-03-29T15_35_03_160_MAX_SPEED/c690ef43-feda-40b7-8f6d-ad22722b3069_j.json"], capture_output=True, text=True)
	# print(result.stdout)
	# output = result.stdout

@app.route('/speedControl/test', methods = ['GET'])
def getTestData():
	print("test")
	# t = Thread(target=runExe,args=())
	# t.start()
	# result = subprocess.run(["./build/speedEstimation_v3", "/media/sd/speedEstimationFiles/AZMES_ME06EKIN_2023-03-29T15_35_03_160_MAX_SPEED/c690ef43-feda-40b7-8f6d-ad22722b3069_j.json"], capture_output=True, text=True)
	# result = subprocess.run(["./build/speedEstimation_v3", "/media/sd/speedEstimationFiles/ekinfixedpointspeedestimation/calibration_3/data/projection_mat.txt","/media/sd/speedEstimationFiles/ekinfixedpointspeedestimation/calibration_3/data/plt-gg_homography_matrix.txt","/media/sd/speedEstimationFiles/AZMES_ME06EKIN_2023-03-29T15_35_03_160_MAX_SPEED/c690ef43-feda-40b7-8f6d-ad22722b3069_j.json","/media/sd/speedEstimationFiles/ekinfixedpointspeedestimation/calibration_3/data/corresponding_points.txt"], capture_output=True, text=True)

	# print(result.stdout)
	# output = result.stdout
	# output = subprocess.call(["./build/speedEstimation_v3", "/media/sd/speedEstimationFiles/AZMES_ME06EKIN_2023-03-29T15_35_03_160_MAX_SPEED/c690ef43-feda-40b7-8f6d-ad22722b3069_j.json"])
	output = subprocess.Popen(["./build/speedEstimation_v3", "/media/sd/speedEstimationFiles/AZMES_ME06EKIN_2023-03-29T15_35_03_160_MAX_SPEED/c690ef43-feda-40b7-8f6d-ad22722b3069_j.json"], stdout=subprocess.PIPE).communicate()
	# outputStr = str(output)
	# wrongIndex = outputStr.find("vehicle")
	# print(wrongIndex)
	# return outputStr[wrongIndex:]
	return "asdf"

serve(app, host = "0.0.0.0", port = 9191)

