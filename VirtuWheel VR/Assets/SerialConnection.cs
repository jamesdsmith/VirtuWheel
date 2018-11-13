using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[Serializable]
public class Message
{
    public int version;
    public List<int> touch_points;
}

public class SerialConnection : MonoBehaviour {

    public GameObject obj;

    System.IO.Ports.SerialPort sp;
	// Use this for initialization
	void Start () {
        print("Serial started");
        sp = new System.IO.Ports.SerialPort("/dev/tty.usbmodem14441", 115200, 
                                            System.IO.Ports.Parity.None, 8, 
                                            System.IO.Ports.StopBits.One);
        if (sp != null) {
            sp.Open();
        } else {
            print("Couldn't open the USB port");
        }

        if (obj != null) {
            obj.SetActive(false);
        }


    }
	
	// Update is called once per frame
	void Update () {
        if (sp != null)
        {
            while (sp.BytesToRead > 0)
            {
                try {
                    var readData = sp.ReadLine();
                    Message msg = JsonUtility.FromJson<Message>(readData);
                    print(msg.touch_points[0]);
                    if (msg.touch_points[0] > 0) {
                        obj.SetActive(true);
                    }
                    if (msg.touch_points[0] <= 0) {
                        obj.SetActive(false);
                    }
                }
                catch(ArgumentException ex){

                }

            }
        }
    }
}
