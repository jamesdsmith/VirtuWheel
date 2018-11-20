using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TouchInteraction : MonoBehaviour
{
    public SerialConnection<Message> serial;
    public string device = "/dev/tty.usbmodem14441";
    public int baudRate = 115200;

    void Start()
    {
        serial = new SerialConnection<Message>(device, baudRate);
        if (serial != null)
        {
            serial.ReceiveData += Serial_ReceiveData;
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

    private void Serial_ReceiveData(Message msg)
    {
        List<float> data = msg.touch_points;
        throw new System.NotImplementedException();
    }
}
