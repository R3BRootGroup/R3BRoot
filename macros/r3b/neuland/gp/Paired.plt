# line styles for ColorBrewer Paired
# for use with qualitative/categorical data
# provides 8 colors in 4 pairs
# compatible with gnuplot >=4.2
# author: Anna Schneider

# line styles
set linetype 1 lc rgb '#A6CEE3' # light blue
set linetype 2 lc rgb '#1F78B4' # dark blue
set linetype 3 lc rgb '#B2DF8A' # light green
set linetype 4 lc rgb '#33A02C' # dark green
set linetype 5 lc rgb '#FB9A99' # light red
set linetype 6 lc rgb '#E31A1C' # dark red
set linetype 7 lc rgb '#FDBF6F' # light orange
set linetype 8 lc rgb '#FF7F00' # dark orange

# palette
set palette maxcolors 8
set palette defined ( 0 '#A6CEE3',\
    	    	      1 '#1F78B4',\
		      2 '#B2DF8A',\
		      3 '#33A02C',\
		      4 '#FB9A99',\
		      5 '#E31A1C',\
		      6 '#FDBF6F',\
		      7 '#FF7F00' )
