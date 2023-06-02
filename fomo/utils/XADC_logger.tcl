	proc write_xadc {end_time} {
		#Set here the directory and file name
		#set filename C:/Users/maga/Desktop/XADC_dump.csv
		set filename /home/maga/work/utils/power/XADC_metrics.csv
		set fileId [open $filename "w"]
		puts "Starting"
		set end 0
		puts $fileId "Time,Temp,Vext,Iext,Vcore,Icore"
		while {$end < $end_time} {
			#the number 1000 indicates the loop will be run every second (1000 ms).
			#If you need a higher or lower frequency, adjust it.
			after 100
			
			set systemTime [clock seconds]
			puts -nonewline $fileId [clock format $systemTime -format %H:%M:%S]
			puts -nonewline $fileId ","

			#this is the XADC properties that you want to have printed.
			#Add as many as you need.
			puts -nonewline $fileId [get_property TEMPERATURE [get_hw_sysmons]]
			puts -nonewline $fileId ","
			puts -nonewline $fileId [get_property VAUXP1_VAUXN1 [get_hw_sysmons]]
			puts -nonewline $fileId ","
			puts -nonewline $fileId [get_property VAUXP9_VAUXN9 [get_hw_sysmons]]
			puts -nonewline $fileId ","
			puts -nonewline $fileId [get_property VCCINT [get_hw_sysmons]]
			puts -nonewline $fileId ","
			puts -nonewline $fileId [get_property VAUXP10_VAUXN10 [get_hw_sysmons]]

			puts $fileId " "
			flush stdout
			incr end
		}
		close $fileId
		puts "Ended"
	}

#The number 10 calls the function write_xadc and have the loop executed 10 times.
#The multiplication time XADC * loop time will tell you how moany logs and for how long the process will be run.
#In this case, 10 times * 1 log/second = 10 seconds = 10 logs.
#write_xadc 10

