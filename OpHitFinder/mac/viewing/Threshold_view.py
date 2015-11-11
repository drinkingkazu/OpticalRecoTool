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


if(my_module.PulseRecoName() != 'Threshold'):
   raise Exception('\n\n\n\tPulse reco needs to be: Threshold. You gave me %s \n\n\n' % my_module.PulseRecoName()) 

cc = 0
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

for opdigit_index in xrange(br.size()):
    print '  Reading OpDigit index',opdigit_index

    opdigit = br[opdigit_index]
    opch = opdigit.ChannelNumber()
 
    if opch > 32: continue

    if not opdigit.size(): continue

    pmt_id = larutil.Geometry.GetME().OpDetFromOpChannel(opch)

    print
    print 'PMT:',pmt_id,'Ch.:',opdigit.ChannelNumber(),'Size:',opdigit.size(),'Time:',opdigit.TimeStamp()
    print

    pulses  = my_module.Reconstruct(opdigit)
    pulses = my_module.Pulses()
    print 'Found',pulses.size(),'pulses!'

    for p in pulses:

        fig,ax = plt.subplots(figsize=(10,6))

        wf = np.array(opdigit.Waveform())

        plt.plot(np.arange(0,opdigit.size(),1),wf,marker='o',linestyle='--',color='black')
        plt.errorbar(np.arange(0,opdigit.size(),1),my_module.PedestalMean(),yerr=my_module.PedestalSigma(),
                     marker='o',linestyle='-',color='red')

        ymax = wf.max()
        ymin = wf.min()

        xmin = 0
        xmax = len(opdigit)
        upstream = 2043

        pmean = np.array(my_module.PedestalMean())

        plt.grid()
        plt.xlabel('Time Tick [15.6 ns]',fontsize=20)
        plt.ylabel('ADC',fontsize=20)

        ax.xaxis.set_tick_params(labelsize=18)
        ax.yaxis.set_tick_params(labelsize=18)

        legend = ax.legend(fontsize=32,numpoints=1)
        plt.tight_layout()

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
        
        # ax.text (p.t_cfdcross - 15,upstream + 2,"Const. Frac. Disc.")
        # ax.text (p.t_cfdcross + 1,upstream - 5 ,"Zero pt. crossing\n identifies pulse")
        # ax.arrow(p.t_cfdcross, upstream, 0, p.ped_mean - upstream, head_width=0.75, head_length=0.5, fc='k', ec='k')

        #this will change depending on theshold, i just take default parameters and mirror what is in algo
        threshold = np.array(my_module.PedestalSigma())[0] * 5
        if threshold < 3:
           threshold = 3

           thresh = plt.plot(np.arange(0,wf.size,1),(threshold + p.ped_mean)*np.ones(wf.size),color='blue',lw=5,alpha=0.5)
        plt.legend(handles=[start,end,peak,area],loc='best')
        ax.set_xlim(p.t_start - 15,p.t_end+25)
        ax.set_ylim(p.ped_mean - 5,p.ped_mean+p.peak + 2)

        bboxx=dict(facecolor='white', edgecolor='black', boxstyle='round,pad=0.5',alpha=0.9)
        ax.text(p.t_start-13,p.ped_mean + 5,"Threshold level\nsignals new pulse",bbox=bboxx)
        plt.title("Pulse Reco: Threshold")

        ax.set_xlabel('Time Tick [15.6 ns]')
        ax.set_ylabel('ADC')
        
        #plt.savefig('Threshold_%d.eps' % cc, format='eps', dpi=1000)

        plt.show()

        cc += 1
        
        
