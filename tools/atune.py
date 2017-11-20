

class PID_ATune:
    def __init__(self, input, output):
        self.isMax = False
        self.isMin = False
        self.input = input
        self.output = output
        self.setpoint = 0.0
        self.noiseBand = 0.0
        self.controlType = 0
        self.running = False
        self.peak1 = 0
        self.peak2 = 0
        self.lastTime = 0
        self.sampleTime = 0
        self.nLookBack = 0
        self.peakType = 0
        self.lastInputs = [0.0,]*101
        self.peaks[0.0,]*10
        self.peakCount = 0
        self.justchanged = False
        self.justevaled = False
        self.absMax = 0.0
        self.absMin = 0.0
        self.oStep = 0.0
        self.outputStart = 0.0
        self.Ku = 0.0
        self.Pu = 0.0

    def FinishUp(self):
        pass

    def Runtime(self):
        pass

	def Cancel(self):
        pass	
	
	def SetOutputStep(self, value):
        pass

	def GetOutputStep(self):
        pass
	
	def SetControlType(self, value):
        pass

	def GetControlType(self):
        pass			
	
	def SetLookbackSec(self, value):
        pass

	def GetLookbackSec(self):
        pass
	
	def SetNoiseBand(self, value):
        pass
        
	def GetNoiseBand(self):
        pass
	
	def GetKp(self):
        pass

	def GetKi(self):
        pass

	def GetKd(self):
        pass
