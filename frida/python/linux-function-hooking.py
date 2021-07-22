import frida
import sys

print("JIHOI FRIDA SCRIPT")
### PID or Process, Mode, Module, Function, #args
### preDefined =["4873","3","hi","f",2]
preDefined = []

template = """
	const mode = "%s";
	const arg1 = "%s";
	const arg2 = "%s";
	const arg3 = "%s";

	if(mode == "1")
	{
		console.log("-------------------------------------------")
		console.log("Print All Modules")
		var modules = Process.enumerateModules()
		for (var module of modules)
		{
			console.log("Module Name:" + module.name)
		}
		console.log("-------------------------------------------")
	}

	if(mode == "2")
	{
		console.log("-------------------------------------------")
		console.log("Print All Functions in ["+arg1+"]")
		var modules = Process.enumerateModules()
		var targetModule = []
		for (var module of modules)
		{
			if (module.name == arg1)
				targetModule.push(module)
		}

		if(targetModule.length == 0)
		{
			console.log("Cannot find such Module")
		}
		else if(targetModule.length > 2)
		{
			console.log("Ambigous Module Name")
		}
		else
		{
			var module = targetModule[0]
			var moduleName = module.Name
			var symList = module.enumerateSymbols()

			for( var sym of symList )
			{
				if(sym.type == "function")
				{
					var addressStr = sym.address.toString()
					var margin = ""
					for (var i = 0; i < (16 - addressStr.length); i++) margin += " "
					console.log(" | " + addressStr + margin +"| " + sym.name)
				}
			}
		}

		console.log("-------------------------------------------")
	}

	if(mode == "3")
	{
		console.log("-------------------------------------------")
		console.log("Hooking function ["+ arg2 + "] in ["+ arg1 +"]")
		var modules = Process.enumerateModules()
		var targetModule = []
		var targetFunction = []

		for (var module of modules)
		{
			if (module.name == arg1)
				targetModule.push(module)
		}

		if(targetModule.length == 0)
		{
			console.log("Cannot find such Module")
		}
		else if(targetModule.length > 2)
		{
			console.log("Ambigous Module Name")
		}
		else
		{
			var module = targetModule[0]
			var moduleName = module.Name
			var symList = module.enumerateSymbols()

			for( var sym of symList )
			{
				if(sym.type == "function")
				{
					if(sym.name == arg2)
						targetFunction.push(sym)
				}
			}
		}
		
		if(targetFunction.length == 0)
		{
			console.log("Cannot find such Function")
		}
		else if(targetFunction.length > 2)
		{
			console.log("Ambigous Function Name")
		}
		else
		{
			var func = targetFunction[0];
			var funcAddress = func.address
			if(sym.address.isNull())
			{
				console.log("Function address is Null")
			}
			else
			{
				var stringAddress = funcAddress.toString();
				var EventHandler =
				{
					onEnter: function(args)
					{
						console.log("OnEnter [%s]")		//arg2
						var temp = parseInt("%s")	//arg3
						for (var i = 0; i<temp; i++)
						{

							var mem;
							var str;
							try{
								mem = args[i].readPointer()
							}
							catch(e)
							{
								mem = 0
							}
							try{
								str = args[i].readCString()
							}
							catch(e)
							{
								str = 0
							}
							
							if(mem == 0)
								console.log(" - Arg ["+i+"] = "+args[i] +" | (int)"+ args[i].toInt32())
							else if(mem != 0 && str == 0)
								console.log(" - Arg ["+i+"] = "+args[i] +" | (int)"+ args[i].toInt32()+" \t> (ReadMem)"+ mem)
							else if(mem != 0 && str != 0)
							{
								console.log(" - Arg ["+i+"] = "+args[i] +" | (int)"+ args[i].toInt32()+" \t> (ReadMem)"+ mem + " \t| (ReadStr)"+str.slice(0,20))
							}
						}
					},

					onLeave: function(retval)
					{
						console.log("OnLeave [%s]")		//arg2
						console.log(" - Return with " + retval)
						console.log("")
					}
				}
				
				try{
					Interceptor.attach(ptr(stringAddress),EventHandler);
					console.log("Interceptor attached on ["+ func.name + "] on [" + stringAddress + "]")
							
				}
				catch (e)
				{
					console.log(e)
					console.log("Cannot attach Interceptor on [" + func.name +"]")
				}
			}
		}

		console.log("-------------------------------------------")
	}

"""

args = []
session = None

if(len(preDefined) == 0) :
	program = input(" - Process name or PID\n")

	try :
		session = frida.attach(program)
	except Exception as e1 :
		try :
			pid = int(program)
			session = frida.attach(pid)
		except ValueError as e2 :
			print("- Ambigous Process Name, or No such Process.")
			print(e1)
			quit()
		except Exception as e3 :
			print("- No such PID")
			print(e3)
			quit()	

	mode = 0
	while True :
		print("----------------------------------------")
		print("Mode [1] : List All Modules in Process")
		print("Mode [2] : List All Functions in a Module")
		print("Mode [3] : Hooking a Function in a Program")

		mode_str = input("- Choose Mode : \n")
		try :
			mode = int(mode_str)
			break
		except ValueError as e :
			print("- Choose Model With only Integer 1,2,3.")

	moduleName = ""
	if(mode > 1) :
		moduleName = input("- Module Name for Mode [%s]\n" % mode)
	functionName = ""
	inputNum = "0"
	if(mode >2) :
		functionName = input("- Function Name for Mode [%s]\n" % mode)
		inputNum = input("- Function's argument for Hooking Mode [%s]\n" % mode)

	args.append(str(mode))
	args.append(moduleName)
	args.append(functionName)
	args.append(inputNum)

elif (len(preDefined) == 5) :
	print("Process(PID) : %s" % preDefined[0])
	program = preDefined[0]

	try :
		session = frida.attach(program)
	except Exception as e1 :
		try :
			pid = int(program)
			session = frida.attach(pid)
		except ValueError as e2 :
			print("- Ambigous Process Name, or No such Process.")
			print(e1)
			quit()
		except Exception as e3 :
			print("- No such PID")
			print(e3)
			quit()	

	print("Mode : %s" % preDefined[1])
	if(int(preDefined[1]) > 1) :
		print("Module Name : %s" % preDefined[2])
	if(int(preDefined[1]) > 2) :
		print("Function Name: %s" % preDefined[3])
		print("Function Arg Number: %s" % preDefined[4])

	args.append(preDefined[1])
	args.append(preDefined[2])
	args.append(preDefined[3])
	args.append(preDefined[4])


else : 
	print("Wrong Predefined Inputs")
	print("----------------------------------------")
	quit()

script_args = (args[0], args[1], args[2], args[3], args[2], args[3], args[2])
print("----------------------------------------")

js = template % script_args
script = session.create_script(js)

script.load()
sys.stdin.read()
