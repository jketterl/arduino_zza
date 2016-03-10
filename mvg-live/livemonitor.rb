require 'mvg/live'
require 'json'
require 'serialport'

station = 'Sendlinger Tor'

lines = JSON.parse(File.read('../lines.json'))
destinations = JSON.parse(File.read('../destinations.json'))
output = SerialPort.new('/dev/ttyACM0', {:baud => 9600})
trap("INT") {
	output.close()
	exit
}

# initialize displays
sleep(5)

output.write("home\n")
sleep(5)

while true
	puts 'updating display...'
	result = MVG::Live.fetch station, { :transports => [ :u ] }
	filtered = result.select do |train|
		lines.key?(train[:line]) && destinations.key?(train[:destination]) && train[:minutes] <= 5
	end

	if filtered.empty?
		puts "no displayable train found :("
		output.write("0,0\n")
	else
		train = filtered[0]
		puts "displaying train: #{train}"
		lineIndex = lines[train[:line]]
		destinationIndex = destinations[train[:destination]]
		output.write("#{destinationIndex},#{lineIndex}\n")
	end

	sleep(60)
end
