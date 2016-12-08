import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk

# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

my_proc.set_output_file('out.root')

# Specify output root file name
my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
#my_proc.add_process(fmwk.ana_base())
my_proc.add_process(fmwk.MCFlashFinder())

my_module1 = fmwk.CheatFlashFinder()
#my_module1.SetIOProducer("OpHitFinder","cheatFlashBeam")
my_module1.SetIOProducer("ophitBeam","cheatFlashBeam")
my_module1.IgnoreChannelAbove(31)
my_module1.IgnoreChannelBelow(0)
my_proc.add_process(my_module1)

my_module2 = fmwk.CheatFlashFinder()
#my_module2.SetIOProducer("OpHitFinder","cheatFlashCosmic)"
my_module2.SetIOProducer("ophitCosmic","cheatFlashCosmic")
my_module2.IgnoreChannelAbove(231)
my_module2.IgnoreChannelBelow(200)
my_module2.EmulateFEM(True)
my_proc.add_process(my_module2)

#my_module3 = fmwk.FlashFinder()
#my_module3.Configure("beamflash_mc.fcl")
#my_proc.add_process(my_module3)

#my_module4 = fmwk.FlashFinder()
#my_module4.Configure("cosmicflash_mc.fcl")
#my_proc.add_process(my_module4)

my_module5 = fmwk.FlashComp()
my_module5.TreePrefix("beam")
my_module5.SetProducers("cheatFlashBeam","","simpleFlashBeam")
my_proc.add_process(my_module5)

my_module6 = fmwk.FlashComp()
my_module6.TreePrefix("cosmic")
my_module6.SetProducers("cheatFlashCosmic","","simpleFlashCosmic")
my_proc.add_process(my_module6)
#my_proc.set_verbosity(0)

print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
my_proc.run(0,1)

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
