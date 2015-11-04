# for processing
import sys
from ROOT import TChain#, pmtana
from larlite import larlite as fmwk
# for plotting
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.colors as mpc
import matplotlib.dates as dts
import numpy as np
import os
plt.rcParams.update({'font.size': 16})

if not len(sys.argv) == 3:
    print 'Usage: %s OpDetWaveformProducer InputFile'
    sys.exit(1)

producer = sys.argv[1]
in_fname = sys.argv[2]
print
print 'Input info ... producer =',producer,' : file =',in_fname
print

my_module = fmwk.OpHitFinder()
my_module.Configure("ophitfindermodule.fcl")
my_module.initialize()

ch=TChain("opdigit_%s_tree" % producer)
ch.AddFile(in_fname)

if ch.GetEntries() < 1:
    print 'TChain not found or empty...'
    sys.exit(1)
    
print 'Found',ch.GetEntries(),'entries...'

for entry in xrange(ch.GetEntries()):

    print 'Reading entry',entry

    ch.GetEntry(entry)

    br = None
    exec('br = ch.opdigit_%s_branch' % producer)

    for opdigit_index in xrange(br.size()):

        print '  Reading OpDigit index',opdigit_index

        opdigit = br[opdigit_index]

        print
        print '    Ch.:',opdigit.ChannelNumber(),'Size:',opdigit.size(),'Time:',opdigit.TimeStamp()
        print

        pulses = my_module.Reconstruct(opdigit)
        print '    Found',pulses.size(),'pulses!'

        fig,ax = plt.subplots(figsize=(10,6))

        # Create np.array instead of using opdigit as is (which is totally fine) to get useful np.array functions
        wf = np.array(opdigit)
        
        plt.plot(np.arange(0,opdigit.size(),1),wf,marker='o',linestyle='--')

        ymax = wf.max()
        ymin = wf.min()

        xmin = 0
        xmax = len(opdigit)
        for p in pulses:

            span_xmin = p.t_start / len(opdigit)
            span_xmax = p.t_end / len(opdigit)
            
            span_ymax = p.peak + p.ped_mean
            span_ymin = p.ped_mean

            plt.axhspan(xmin = span_xmin,
                        xmax = span_xmax,
                        ymin = span_ymin,
                        ymax = span_ymax,
                        color='orange',lw=2,alpha=0.5)
            print span_xmin, span_xmax, span_ymin, span_ymax

            #if xmin == 0: xmin = span_xmin
            #if xmax == len(opdigit): xmax = span_xmax

        plt.grid()
        plt.xlabel('Time Tick [15.6 ns]',fontsize=20)
        plt.ylabel('ADC',fontsize=20)
        plt.ylim(ymin-5,ymax+5)
        #plt.xlim(0,len(wf))
        plt.xlim(xmin,xmax)
        ax.xaxis.set_tick_params(labelsize=18)
        ax.yaxis.set_tick_params(labelsize=18)
        
        #ax.xaxis.labelpad=12
        #ax.yaxis.labelpad=12
        legend = ax.legend(fontsize=32,numpoints=1)
        #legend.get_frame().set_facecolor('#FFFFFF')
        plt.tight_layout()
        #plt.savefig('neutrino_bump_50.eps',format='eps',dpi=20000)
        plt.show()

    break
