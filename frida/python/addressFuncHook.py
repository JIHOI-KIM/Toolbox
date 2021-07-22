import frida
import sys


pid = input("| Session Name of PID?\n")
address = input("| Address of the Hooking Point?\n")

try :
	pid = int(pid)
except ValueError as e2 :
	print("Treat as Program Name, not PID...")
	pass

session = frida.attach(pid)


script = session.create_script("""
Interceptor.attach(ptr("%s"), {
    onEnter: function(args) {

		for (var i =0; i<4; i++){
        	console.log("Arg["+i+"] : (int)" + args[i].toInt32());
			try
			{
				mem = args[i].readPointer()
			}
			catch(e)
			{
				mem = 0
			}
			try
			{
				str = args[i].readCString()
			}
			catch(e)
			{
				str = 0
			}
		
        	console.log("Arg["+i+"] : (mem)" + args[i].readPointer());
        	console.log("Arg["+i+"] : (str)" + args[i].readCstring());
    	}
	}
});
""" % int(address, 16))

script.load()
sys.stdin.read()
