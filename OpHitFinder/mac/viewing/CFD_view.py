import matplotlib.pyplot  as plt
import matplotlib.colors  as mpc
import matplotlib.patches as patches

import numpy as np
import ROOT  as rt
from ROOT import larutil
from larlite import larlite as fmwk
plt.rcParams.update({'font.size': 16})

import sys

if not len(sys.argv) == 3:
   print 'Usage: %s OpDetWaveformProducer InputFile'
   sys.exit(1)
    

producer = sys.argv[1]
in_fname = sys.argv[2]

my_module = fmwk.OpHitFinder()                                                                                           
my_module.Configure("../ophitfindermodule.fcl")
my_module.initialize()


if(my_module.PulseRecoName() != 'CFD'):
   raise Exception('\n\n\n\tPulse reco needs to be: CFD. You gave me %s \n\n\n' % my_module.PulseRecoName()) 


ch = rt.TChain("opdigit_%s_tree" % producer)
ch.AddFile(in_fname)


if ch.GetEntries() < 1:
    print 'TChain not found or empty...'
    sys.exit(1)
    
print 'Found',ch.GetEntries(),'entries...'


# In[ ]:

entry = 0;
#for entry in xrange(1,2):
print 'Reading entry',entry

ch.GetEntry(entry)

br = None
exec('br = ch.opdigit_%s_branch' % producer)

cc = 0;

for opdigit_index in xrange(br.size()):
   print '  Reading OpDigit index',opdigit_index
   
   opdigit = br[opdigit_index]
   opch = opdigit.ChannelNumber()
   
   #if opch > 32: continue
   if opch != 11: continue
   
   if not opdigit.size(): continue
   
   pmt_id = larutil.Geometry.GetME().OpDetFromOpChannel(opch)
   
   print
   print 'PMT:',pmt_id,'Ch.:',opdigit.ChannelNumber(),'Size:',opdigit.size(),'Time:',opdigit.TimeStamp()
   print
   
   pulses  = my_module.Reconstruct(opdigit)
   pulses = my_module.Pulses()
   print 'Found',pulses.size(),'pulses!'
   
   fig,ax = plt.subplots(figsize=(10,6))
   
   
   wf = np.array(opdigit.Waveform())
   
   ax.plot(np.arange(0,opdigit.size(),1),wf,marker='o',linestyle='--',color='black')
   ax.errorbar(np.arange(0,opdigit.size(),1),my_module.PedestalMean(),yerr=my_module.PedestalSigma(),
               marker='o',linestyle='-',color='red')
   
   ymax = wf.max()
   ymin = wf.min()
   
   xmin = 0
   xmax = len(opdigit)
   upstream = 2043
   
   pmean = np.array(my_module.PedestalMean())
   cfd = []
   
   for i in xrange(len(wf)):
      f = -1.0 * 0.9 * (wf[i] - pmean[i])
      if i < 2:
         cfd.append(f)
      else:
         cfd.append(f + wf[i-2] - pmean[i])
         
   cfd = np.array(cfd)
   #upstream = np.min(wf[p.t_start:p.t_end]) - np.max(cfd[p.t_start:p.t_end])-3
   upstream=pmean-50
   cfd += upstream
   
   ax.plot(np.arange(0,wf.size,1),cfd,lw=2)
   ax.plot(np.arange(0,wf.size,1),upstream*np.ones(wf.size))
   
   plt.grid()
   ax.set_xlabel('Time Tick [15.6 ns]',fontsize=20)
   ax.set_ylabel('ADC',fontsize=20)
   
   ax.xaxis.set_tick_params(labelsize=18)
   ax.yaxis.set_tick_params(labelsize=18)
   
   legend = ax.legend(fontsize=32,numpoints=1)
   plt.tight_layout()
   
   for ix,p in enumerate(pulses):
      print ix,p
      fm = plt.get_current_fig_manager()
      fm.canvas.figure = fig
      fig.canvas = fm.canvas
      
      span_xmin = p.t_start
      span_xmax = p.t_end
      
      span_ymax = p.peak + p.ped_mean
      span_ymin = p.ped_mean
      
      a = []
      a.append((p.t_start,p.ped_mean))
      
      for i in xrange(int(p.t_start + 1),int(p.t_end)+1):
         a.append((i,wf[i]))
         
      a.append((p.t_end,p.ped_mean))
      a.append((p.t_start,p.ped_mean))
      area = ax.add_patch(patches.Polygon(a,color='orange',alpha=0.2,label="Area: %d" % p.area))
            
      #lines on each side of pulse
      start = ax.vlines(p.t_start,p.ped_mean,p.peak + p.ped_mean,colors='magenta',linestyles='dashed',lw=2,label='Start tick: %d' % p.t_start)
      
      #lines on each side of pulse
      end = ax.vlines(p.t_end,p.ped_mean,p.peak + p.ped_mean,colors='green',linestyles='dashed',lw=2,label='End tick: %d' % p.t_end)
      
      #lines on top for peak
      peak = ax.hlines(p.peak + p.ped_mean,p.t_start,p.t_end,colors='gray',linestyles='dashed',lw=2,label='Peak: %d' % p.peak)
      
      # CFD will be here where we make the fake waveform under it
      #ax.vlines(p.t_cfdcross,p.ped_mean-2,p.ped_mean+2,colors='blue',lw=2)
      ax.vlines(p.t_max,p.ped_mean-2,p.ped_mean+2,colors='blue',lw=2)
      bboxx=dict(facecolor='white', edgecolor='black', boxstyle='round,pad=0.5',alpha=0.9)
      
   ax.set_xlim(0,1500)
   ax.set_ylim(0,4500)
        
   ax.set_title("Pulse Algo. CFD")
   plt.show()
   #plt.savefig('CFD_%d.eps' % cc, format='eps', dpi=1000)
   cc+=1
