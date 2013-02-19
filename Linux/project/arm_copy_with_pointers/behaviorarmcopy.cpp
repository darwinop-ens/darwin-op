class BehaviorArmCopy : public AbstractBehavior
{
	private:
		CM730* cm730;

	public:
	
	
	
	BehaviorArmCopy(CM730* cm730arg)
	{
		this->cm730 = cm730arg;
	}
	
	
	
	private void initialize()
	{
		  printf("Try to connect to the robot...\n");
		  if(cm730->Connect() == false)
		  {
			printf("ERROR: impossible to connect to the robot\n");
			return;
		  }
		  printf("Connexion succeeded\n");

		  cm730->WriteByte(JointData::ID_L_ELBOW, MX28::P_TORQUE_ENABLE, 0, 0);
		  cm730->WriteByte(JointData::ID_L_SHOULDER_ROLL, MX28::P_TORQUE_ENABLE, 0, 0);
		  cm730->WriteByte(JointData::ID_L_SHOULDER_PITCH, MX28::P_TORQUE_ENABLE, 0, 0);
		  printf("Bras gauche debloque\n");
		  cm730->WriteByte(JointData::ID_R_ELBOW, MX28::P_TORQUE_ENABLE, 1, 0);
		  cm730->WriteByte(JointData::ID_R_SHOULDER_ROLL, MX28::P_TORQUE_ENABLE, 1, 0);
		  cm730->WriteByte(JointData::ID_R_SHOULDER_PITCH, MX28::P_TORQUE_ENABLE, 1, 0);
		  printf("Bras droit bloque\n");
	}
	
	
	
	void oneStep()
	{
		  int v;

		  if(cm730->ReadWord(JointData::ID_L_ELBOW, MX28::P_PRESENT_POSITION_L, &v, 0) == CM730::SUCCESS)
		  {
			cm730->WriteWord(JointData::ID_R_ELBOW, MX28::P_GOAL_POSITION_L, 4096-v, 0);
		  }
		  else
		  {
			printf("can not read the present position of left elbow\n");
		  }

		  if(cm730->ReadWord(JointData::ID_L_SHOULDER_ROLL, MX28::P_PRESENT_POSITION_L, &v, 0) == CM730::SUCCESS)
		  {
			cm730->WriteWord(JointData::ID_R_SHOULDER_ROLL, MX28::P_GOAL_POSITION_L, 4096-v, 0);
		  }
		  else
		  {
			printf("can not read the present position of left shoulder (pitch)\n");
		  }

		  if(cm730->ReadWord(JointData::ID_L_SHOULDER_PITCH, MX28::P_PRESENT_POSITION_L, &v, 0) == CM730::SUCCESS)
		  {
			cm730->WriteWord(JointData::ID_R_SHOULDER_PITCH, MX28::P_GOAL_POSITION_L, 4096-v, 0);
		  }
		  else
		  {
			printf("can not read the present position of left elbow\n");
		  }
	}

}


