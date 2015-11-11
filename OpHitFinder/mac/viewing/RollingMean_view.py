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

if(my_module.PedAlgoName() != 'PedUB'):
   raise Exception('\n\n\n\tPed algo needs to be: PedUB. You gave me %s \n\n\n' % my_module.PedAlgoName()) 

ch = rt.TChain("opdigit_%s_tree" % producer)
ch.AddFile(in_fname)

if ch.GetEntries() < 1:
    print 'TChain not found or empty...'
    sys.exit(1)
    
print 'Found',ch.GetEntries(),'entries...'



entry = 0;

print 'Reading entry',entry

ch.GetEntry(entry)

br = None
exec('br = ch.opdigit_%s_branch' % producer)
cc = 0
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
    pulses  = my_module.Pulses()
    print 'Found',pulses.size(),'pulses!'

    for p in pulses:

        fig,ax = plt.subplots(figsize=(10,6))

        wf = np.array(opdigit.Waveform())
        pmean = np.array(my_module.PedestalMean())
        psigma = np.array(my_module.PedestalSigma())
        
        waveform     = plt.plot(np.arange(0,opdigit.size(),1),
                                wf,marker='o',
                                linestyle='--',
                                color='black',
                                label='Waveform')
        
        rollingmean  = plt.errorbar(np.arange(0,opdigit.size(),1),
                                    pmean,yerr=psigma,
                                    marker='o',
                                    linestyle='-',
                                    color='red',
                                    label='Rolling Mean')
        continuation = plt.errorbar(np.arange(p.t_start,p.t_end+1,1),
                                    pmean[p.t_start:p.t_end+1],
                                    yerr=psigma[p.t_start:p.t_end+1],
                                    marker='o',
                                    linestyle='-',
                                    color='blue',
                                    label='Continuation')

        ymax = wf.max()
        ymin = wf.min()

        xmin = 0
        xmax = len(opdigit)
        upstream = 2043

        bboxx=dict(facecolor='white', edgecolor='black', boxstyle='round,pad=0.5',alpha=0.9)

        #
        #
        # Re-enable this text to draw info on the plot
        #
        #
        
        
        # ax.text(p.t_start-24,int(p.ped_mean)+6,'Baseline calculated\ntickwise in sliding window.', 
        #         bbox=bboxx)
        # ax.text(p.t_max,int(p.ped_mean)+4,'Baseline is continued under peaks.', 
        #         bbox=bboxx)
        
        #plt.legend(handles=[waveform,rollingmean,continuation],loc='best')
        plt.legend()
        ax.set_xlim(p.t_start-25,p.t_end+25)
        ax.xaxis.grid()  
        ax.yaxis.grid() 

        ax.set_ylim(p.ped_mean - 5,p.ped_mean+p.peak + 2)
        plt.title("Ped Algo. RollingMean")



        ax.set_xlabel('Time Tick [15.6 ns]')
        ax.set_ylabel('ADC')

        #plt.savefig('RollingMean_%d.eps' % cc, format='eps', dpi=1000)

        plt.show()

        cc+=1

