#target = open("/home/pi/Desktop/test42.txt", 'w')
import subprocess


#works

subprocess.Popen('/home/pi/Desktop/gpsCar')
subprocess.Popen('/home/pi/Desktop/wvdialAutostart.sh', shell=True)










#subprocess.Popen('sudo wvdial netzclub &', shell=True)
#subprocess.Popen(['lxterminal', '-e sudo wvdial netzclub &'], shell=True)
#subprocess.check_output(["echo", "Hello World!"], shell=True)
#subprocess.check_output(['lxterminal', '-e sudo wvdial netzclub &'], shell=True)
