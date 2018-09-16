#!/usr/bin/python
#watches a jenkins job and turns off an LED when the last build failed. 
#starts that jenkins job on demand when a physical button is pressed.
from pyA20.gpio import gpio
from pyA20.gpio import port
from time import sleep
import multiprocessing
import requests
import json


#vars
led=port.PA20
button=port.PA10
#debug=True
debug=False

job="myjob"
user="admin"
password="test1234"
auth = (user,password)
base_url = "http://192.168.4.15:8080"
#no params
build_uri = "/job/%s/build" %(job)
#build_uri = "/job/%s/buildWithParameters" %(job)
full_build_url = base_url + build_uri
status_uri = "/job/%s/api/json" %(job)
full_status_url = base_url + status_uri

#setup the pins
gpio.init()
gpio.setcfg(led, gpio.OUTPUT)
gpio.setcfg(button, gpio.INPUT)
gpio.pullup(button, gpio.PULLUP)



def debugging( r ):
  print (r.request.url)
  print (r.request.headers)
  print (r.request.body)
  print (r.headers)
  print(r.status_code)
  print(r.text)

def getcrumb(loud=False):
  crumbreq = requests.get('%s/crumbIssuer/api/xml?xpath=concat(//crumbRequestField,":",//crumb)' %(base_url), auth=auth)
  crumb = crumbreq.text.split(':')[1]
  if loud:
    debugging( crumbreq )
  return crumb

#check job status. turn on light if good. turn off light if bad.
def job_status(crumb, loud=False):
    while True:
        headers = {"Content-Type": "application/json", "Jenkins-Crumb": crumb}
        r = requests.get(full_status_url, auth=auth, headers=headers)
        j = json.loads(r.text)
        if loud:
            debugging( r )
            if j['lastSuccessfulBuild']['number'] == j['lastBuild']['number']:
                print "last job good"
            else:
                print "last job bad"
        if j['lastSuccessfulBuild']['number'] == j['lastBuild']['number']:
            gpio.output(led, gpio.HIGH)
        else:
            gpio.output(led, gpio.LOW)
        sleep(9)

#watch the button and start job when pressed
def watch_button(crumb, loud=False):
    while True:
        #reversed due to pull up
        if not gpio.input(button):
            payload = {"OTHER": "xzy"} #used for parameterized builds
            headers = {"Content-Type": "application/x-www-form-urlencoded", "Jenkins-Crumb": crumb}
            r = requests.post(full_build_url, data=payload, auth=auth, headers=headers)
            if loud:
                debugging( r )
                print "button pressed"
                if r.status_code == 201:
                    print  "build scheduled."
                else:
                    print "failed to schedule build."
            sleep(.5)


if __name__ == "__main__":
    c = getcrumb()
    if debug:
        pjob_status = multiprocessing.Process(target=job_status, args=(c, True))
        pwatch_button = multiprocessing.Process(target=watch_button, args=(c, True))
    else:
        pjob_status = multiprocessing.Process(target=job_status, args=(c,))
        pwatch_button = multiprocessing.Process(target=watch_button, args=(c,))

    pjob_status.start()
    pwatch_button.start()
