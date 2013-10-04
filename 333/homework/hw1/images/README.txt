This directory contains four test images, and four image histograms produced
using our solution on the test images.

You can produce a histogram using the following command pipeline, for
example:

jpegtopnm idaho.jpg | ../solution_binaries/image_hist | ppmtojpeg > idaho_hist.jpg
