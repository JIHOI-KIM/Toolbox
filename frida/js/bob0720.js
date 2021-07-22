console.log("-------------------------------------------")
console.log("Running [BOB FRIDA SCRIPT]")
console.log("-------------------------------------------")

function getRandom(max) {
    return Math.floor(Math.random() * (max));
}

var exitDoor = 0
while( exitDoor == 0 )
{
	exitDoor = 1
	var modules = Process.enumerateModules()
	var targetModule = null
	console.log("[+] Find " + modules.length + " Modules from program.")
	var find = 0
	for (var module of modules)
	{
		if (module.name == "mstscax.dll")
		{
			console.log("[+] ... Find mstscax.dll from Module list.")
			find = 1
			targetModule = module
		}
	}
	if(find == 0 )
	{
		console.log("Cannot find module mstscax.dll")
		break;
	}

	console.log("[INFO] "+targetModule.name+" Have "+targetModule.enumerateExports().length+" Exports and "+targetModule.enumerateImports().length+" Imports.")
	var targetBase = Module.findBaseAddress(targetModule.name)
	console.log("[INFO] "+targetModule.name+ " Base Address " + targetBase)

	console.log("[NOTICE] Cannot Find [ChannelOnPacketReceived()] from Exports.")
	console.log("[NOTICE] Analyze it manually...")
	var targetAdd = ptr(targetBase).add(ptr('0x1913B8'))
	console.log("[NOTICE] Assume "+targetAdd +" is address of ChannelOnPacketReceived().")

	console.log("[+] Attatch Interceptor for " + targetAdd)
	Interceptor.attach(targetAdd, {
		onEnter: function (args)
		{
			console.log("[++] Funtcion [ChannelOnPacketReceived()] is called!")
			console.log("[++] arguments 0 (Hex form) : " + args[0].toString(16))
			console.log("[++] arguments 1 (Hex form) : " + args[1].toString(16))
			console.log("[++] arguments 2 (Hex form) : " + args[2].toString(16))

			console.log("[+++] Do mutation")
			console.log("[+++] Payload Address : " + args[1].toString(16) + " | Length : "+ args[2].toInt32())
			console.log("[+++] Try Payload Mutation ... ")

			var payloadAdd = ptr(args[1])
			var payloadLength = args[2].toInt32()

			if (payloadLength > 48)
			{
				console.log("[NOTICE] Payload Length is too long! (" + payloadLength + ")")
				console.log("[NOTICE] Change it for short view (48)")
				payloadLength = 48
			}

			var original = Memory.readByteArray(payloadAdd, payloadLength);
			console.log("[++++] ORIGINAL RECV")
			console.log(original)

			if( false ) // Change to true for mutation.
			{
				var randomPos = getRandom(payloadLength)
				var randomVal = getRandom(0x100)
				console.log("[++++] Mutation Offset : " + randomPos + " | Mutation Value : " + randomVal)
       			Memory.writeByteArray(payloadAdd.add(randomPos), [randomVal]);
       			var mutation = Memory.readByteArray(payloadAdd, payloadLength);
				console.log("[++++] MUTATION RECV")
				console.log(mutation)
			}
		},
		onLeave: function (retval)
		{
			console.log("[--] Funtcion [ChannelOnPacketReceived()] return!")
			console.log("[--] return value (Hex form) : " + retval.toString(16))
		}
	})

}

if(exitDoor == 0)
{
	console.log("[-] Program End.")
}

if(exitDoor == 1)
{
	console.log("[+] Hooking Done.")
}
