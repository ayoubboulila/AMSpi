'''
Created on 14 f�vr. 2018

@author: AYB
'''
import redis
from datetime import datetime


r = redis.StrictRedis()

now = datetime.now()
r.publish('test', now)

