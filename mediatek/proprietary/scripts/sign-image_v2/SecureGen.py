#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import sys
import os
import struct
import re
import commands
import subprocess
import fileinput
import ntpath



def executeShell(sh_command):
	subprocess.call(sh_command, shell=True)
	
def setPath():
	global out_path
	global resign_tool_path
	global cert1_dir
	global cert2_key_dir
	global img_ver_path
	global bin_tmp_path
	global out_cert1_path

	if(os.environ.get('OUT') == None):
		out = "out/"
	else:
		out = os.environ.get('OUT')+"/"
	out_path = out+"resign/"
	out_cert1_path= 	out_path + "cert/cert1/cert1.der"

	resign_tool_path = "vendor/mediatek/proprietary/scripts/sign-image_v2/signtool/resignTool.py"
	cert1_dir = "vendor/mediatek/proprietary/custom/"+platform+"/security/cert_config/cert1/"
	cert2_key_dir = "vendor/mediatek/proprietary/custom/"+platform+"/security/cert_config/cert2_key/"
	img_ver_path  = "vendor/mediatek/proprietary/custom/"+platform+"/security/cert_config/img_ver.txt"
	

	
	
def fillArgDict(str,key,argDict):
	prefix = str.split("=")[0]
	format = re.compile(key,re.I)
	if(format.search(prefix)):
		val = str.split("=")[1]
		argDict[key] = val
		print key+": "+val
	return argDict
	
def parseArg(argv):
	global platform
	global argDict
	argDict = {'cert2_key_path': 0 , 'cert1_key_path': 0 }
	
	for str in argv:
		for key in argDict:
			argDict = fillArgDict(str,key,argDict)

	#check input
	platform = argv[1]
	#print argDict


def copyFile(path1,path2):
	sh_command = 'cp '+path1+' '+ path2
	subprocess.call(sh_command, shell=True)	
	

def getBinList():
	pattern1= "\["
	format1 = re.compile(pattern1)
	f = open(img_ver_path,'r')
	binList =[]
	for line in f:
		if( not line.strip()):
			continue
	
		if(format1.match(line)):
			bin_name = line[line.index("[")+1:line.index("]")]
			binList.append(bin_name)

	#print keyList
	f.close()
	return binList
	
def genCert2Key(binList):
	print "Start gen cert2 key to " + cert2_key_dir

	for bin in binList:
		key = bin +"_privk2.pem"
		print "Gen "+ key
		copyFile(argDict["cert2_key_path"] , cert2_key_dir+key)
		
def genCert1(binList):

	for bin in binList:
		cert2_key_path = cert2_key_dir+bin+"_privk2.pem"
		sh_command = "python "+ resign_tool_path+ " type=cert1  privk="+argDict["cert1_key_path"] +" pubk="+cert2_key_path
		#print bin_name
		#print img_name
		print sh_command
		executeShell(sh_command)
		cert1_path = cert1_dir + bin+"_cert1.der"
		print cert1_path
		copyFile(out_cert1_path,cert1_path)
		print "--------"

	
def main():

	if(len(sys.argv) < 3):
		print "SecureGen.py <platform> cert1_key=<key_path> cert2_key=<key_path>"
		print "e.x Gen cert1"
		print "    SecureGen.py mt6797 cert1_key=privk1.pem"
		print "e.x Gen cert2 key"
		print "    SecureGen.py mt6797 cert2_key=privk2.pem"
		print "e.x Gen cert1 and gen cert2 key"
		print "    SecureGen.py mt6797 cert1_key=privk1.pem cert2_key=privk2.pem"
		sys.exit()
	parseArg(sys.argv)
	
	setPath()
	
	binList = getBinList()
	
	if(argDict["cert2_key_path"] !=0) :
		if( os.path.isfile(argDict["cert2_key_path"]) ):
			genCert2Key(binList)
		else:
			print "cert2_key not exists, please check cert2 key path!"
		
	if(argDict["cert1_key_path"] !=0):
		if( os.path.isfile(argDict["cert1_key_path"]) ):
			genCert1(binList)
		else:
			print "cert1_key not exists, please check cert1 key path!"
		

	sys.exit()
	

if __name__ == '__main__':
	main()