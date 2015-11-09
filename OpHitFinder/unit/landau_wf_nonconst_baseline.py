
# coding: utf-8

# In[ ]:

#%matplotlib inline
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt
import matplotlib.colors as mpc
import matplotlib.dates as dts

import matplotlib.patches as patches

import numpy as np
import ROOT  as rt
from larlite import larlite as fmwk

rt.gSystem.Load("libLArLite_DataFormat.so")


# In[ ]:

### This is the random baseline
def get_libaseline_gaus(baseline,mu=0,sigma=1):
    x = -1
    while ( x < baseline ):
        x = int(baseline + np.random.normal(mu,sigma))
    return x

### This is the linear baseline
def get_linear_baseline_gaus(y,start=1800,end=2048,mu=0,sigma=1):
    x = -1
    while ( x < start ):
        x = int(start + (float(end - start))/1500.0 * y + np.random.normal(mu,sigma)) 
    return x

### This is the log baseline
def get_log_baseline_gaus(y,start=1800,end=2048,mu=0,sigma=1):
    
    x = int(start + start * np.log(y+50)/50.0 + np.random.normal(mu,sigma))
    return x


### This is the unit waveform, landau
def get_waveform(amp,sigma,center=None):

    wf = rt.std.vector('short')()
    
    if center is None:
        center = np.random.uniform(0,1500)
        
    for x in xrange(1500):
        j = int(np.floor(float(amp) * rt.TMath.Landau(x,center,sigma)))
        wf.push_back(j)
    
    return wf
    


# In[ ]:

unit_wf       = rt.larlite.opdetwaveform()
unit_baseline = rt.std.vector("short")()

for w in xrange(0,1500):
    unit_wf.push_back(get_log_baseline_gaus(w+1))

#smaller one toward the beginning
wf1 = get_waveform(100,10,200)

#larger one toward the middle
wf2 = get_waveform(200,2,800)

#put one on the first tick
wf3 = get_waveform(150,2,-1)

#put one on the ending pas last tick
wf4 = get_waveform(150,2,1500)

#how about two overlapping just a hair
wf5 = get_waveform(200,2,1110)
wf6 = get_waveform(180,2,1135)

for i in xrange(unit_wf.size()):
    unit_wf[i] += wf1[i]
    unit_wf[i] += wf2[i]
    unit_wf[i] += wf3[i]
    unit_wf[i] += wf4[i]
    unit_wf[i] += wf5[i]
    unit_wf[i] += wf6[i]


# In[ ]:

#a  = np.array(unit_wf.Waveform())
#fig, ax = plt.subplots(figsize=(15,6))
#ax.plot(np.arange(0,a.size,1),a,'-o')
#ax.set_ylim(np.min(a)-1,np.max(a) + 1)
#plt.show()


# In[ ]:

my_module = fmwk.OpHitFinder()                                                                                           
my_module.Configure("../mac/ophitfindermodule.fcl")                                                                             
my_module.initialize()


# In[ ]:

my_module.Reconstruct(unit_wf)


# In[ ]:

pulses = my_module.Pulses()
opdigit = unit_wf

print '    Found',pulses.size(),'pulses!'

fig,ax = plt.subplots(figsize=(10,6))

wf = np.array(opdigit.Waveform())

plt.plot(np.arange(0,opdigit.size(),1),wf,marker='o',linestyle='--',color='black')
plt.errorbar(np.arange(0,opdigit.size(),1),my_module.PedestalMean(),yerr=my_module.PedestalSigma(),
             marker='o',linestyle='-',color='red')

ymax = wf.max()
ymin = wf.min()

xmin = 0
xmax = len(opdigit)
a = 0
b = 0
for p in pulses:
    
    span_xmin = p.t_start
    span_xmax = p.t_end

    span_ymax = p.peak + p.ped_mean
    span_ymin = p.ped_mean


    ax.add_patch(patches.Rectangle((span_xmin, span_ymin),
                                   span_xmax - span_xmin,
                                   span_ymax - span_ymin,
                                   color='orange',alpha=0.5,lw=2))

    print span_xmin, span_xmax, span_ymin, span_ymax
    
    ax.vlines(p.t_cfdcross,2048-5,2048+5,colors='blue')

plt.grid()
plt.xlabel('Time Tick [15.6 ns]',fontsize=20)
plt.ylabel('ADC',fontsize=20)
plt.ylim(ymin-5,ymax+5)

plt.xlim(xmin,xmax)
ax.xaxis.set_tick_params(labelsize=18)
ax.yaxis.set_tick_params(labelsize=18)

legend = ax.legend(fontsize=32,numpoints=1)
plt.tight_layout()
plt.show()


# In[ ]:




# In[ ]:




# In[ ]:




# In[ ]:




# In[ ]:




# In[ ]:




# In[ ]:




# In[ ]:



