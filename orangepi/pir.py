#!/usr/bin/python
#watch for motion and send a SMS via SMTP when detected
#your ISP can't block outgoing SMTP to non-ISP owned servers for this to work
#you must configure your google account like https://www.digitalocean.com/community/tutorials/how-to-use-google-s-smtp-server and https://support.google.com/accounts/answer/6010255?hl=en
#if you get unreliable or slow SMS or hit gmail rate limits switch to twillo or sendpulse or some other SMS gateway
from pyA20.gpio import gpio
from pyA20.gpio import port
from time import sleep
import datetime
import smtplib
from email.mime.text import MIMEText

#vars
sensor=port.PA9 
debug=False
phone = "5555555555@tmomail.net" #see https://en.wikipedia.org/wiki/SMS_gateway#Email_clients for the email domain of your carrier. replace the 555 with your phone number.
gmailuser="user@gmail.com" #your gmail address
gmailpass="password" #your gmail password

#setup the pins
gpio.init()
gpio.setcfg(sensor, gpio.INPUT)

def pir(loud=False):
    sleep(2) #make sure sensor is ready
    if loud:
        print "looking for motion"
    while True:
        if gpio.input(sensor):
            now = datetime.datetime.now()
            event = "motion detected at: %s" %(now.strftime("%Y-%m-%d %H:%M"))
            if loud:
                print event
            #create the email headers and body
            msg = MIMEText(event)
            msg['From'] = gmailuser
            msg['To'] = phone
            msg['Subject'] = event
            text = msg.as_string()
            #send the mail
            server = smtplib.SMTP_SSL( "smtp.gmail.com", 465 )
            server.login(gmailuser, gmailpass)
            server.sendmail(gmailuser, phone, text)
            server.quit()
            sleep(45) #don't SMS multiple times for the same motion event
        else:
            if loud:
                print "no motion detected"
            sleep(1) #rate limiting for when there is no motion to not overwork the pi


if __name__ == "__main__":
    if debug:
        pir(True)
    else:
        pir()
