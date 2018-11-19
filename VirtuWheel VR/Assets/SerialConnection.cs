using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[Serializable]
public class Message
{
    public int version;
    public List<float> touch_points;
}

public class SerialConnection : MonoBehaviour
{
    System.IO.Ports.SerialPort sp;

    public delegate void OnReceiveData(List<float> data);
    public event OnReceiveData ReceiveData;

    void Start()
    {
        try
        {
            sp = new System.IO.Ports.SerialPort("/dev/tty.usbmodem14441", 115200,
                                                System.IO.Ports.Parity.None, 8,
                                                System.IO.Ports.StopBits.One);
            if (sp != null)
            {
                sp.Open();
            }
            else
            {
                print("Couldn't open the USB port");
            }
        }
        catch (Exception e)
        {
            print(e.Message);
            sp = null;
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (sp != null)
        {
            Message msg = null;
            // Keep pulling messages until we get the most up to date one
            while (sp.BytesToRead > 0)
            {
                try
                {
                    var readData = sp.ReadLine();
                    msg = JsonUtility.FromJson<Message>(readData);
                }
                catch (Exception ex)
                {
                    print(ex.Message);
                }
            }
            if (msg != null)
            {
                ReceiveData(msg.touch_points);
            }
        }
    }
}
