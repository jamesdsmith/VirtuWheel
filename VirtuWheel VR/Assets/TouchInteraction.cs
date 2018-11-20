using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class DataMessage
{
    public int version;
    public List<float> touch_points;
}

public class TouchInteraction : MonoBehaviour
{
    public SerialConnection<DataMessage> serial;
    public string device = "/dev/tty.usbmodem14441";
    public int baudRate = 115200;

    void Start()
    {
        serial = new SerialConnection<DataMessage>(device, baudRate);
        if (serial != null)
        {
            serial.ReceiveData += OnReceiveData;
        }
        else
        {
            print("Serial connection is not established with interaction manager.");
        }
    }
    
    void Update()
    {
        serial.Update();
    }

    private void OnReceiveData(DataMessage msg)
    {
        List<float> data = msg.touch_points;
        throw new NotImplementedException();
    }
}
