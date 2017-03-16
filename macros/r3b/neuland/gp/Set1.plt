# line styles for ColorBrewer Set1
# for use with qualitative/categorical data
# provides 8 easy-to-name colors
# compatible with gnuplot >=4.2
# author: Anna Schneider

# line styles
set linetype 1 lc rgb '#E41A1C' # red
set linetype 2 lc rgb '#377EB8' # blue
set linetype 3 lc rgb '#4DAF4A' # green
set linetype 4 lc rgb '#984EA3' # purple
set linetype 5 lc rgb '#FF7F00' # orange
set linetype 6 lc rgb '#FFFF33' # yellow
set linetype 7 lc rgb '#A65628' # brown
set linetype 8 lc rgb '#F781BF' # pink

# palette
set palette maxcolors 8
set palette defined ( 0 '#E41A1C',\
    	    	      1 '#377EB8',\
		      2 '#4DAF4A',\
		      3 '#984EA3',\
		      4 '#FF7F00',\
		      5 '#FFFF33',\
		      6 '#A65628',\
		      7 '#F781BF' )
