package com.me;

import javax.swing.*;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import java.awt.*;

public class MainWindow {

    public MainWindow()
    {
        textDebug.setText("");
    }

    public void printMessage(String s)
    {
        StyledDocument doc = textDebug.getStyledDocument();
        SimpleAttributeSet keyWord = new SimpleAttributeSet();
        //StyleConstants.setForeground(keyWord, Color.RED);
        //StyleConstants.setBackground(keyWord, Color.YELLOW);
        //StyleConstants.setBold(keyWord, true);
        try
        {
            doc.insertString(doc.getLength(), s, keyWord );

            //panel.revalidate();
            //JScrollBar vertical = debugScrollPane.getVerticalScrollBar();
            //vertical.setValue(vertical.getMaximum());
        }
        catch(Exception e) { System.out.println(e); }
    }

    public boolean isModePosition() {
        return tabbedPane.isEnabledAt(0);
    }

    public boolean isModeDirect() {
        return tabbedPane.isEnabledAt(1);
    }

    public float getDirectBaseRotation() {
        return sliderDirectRotation.getValue();
    }

    public float getDirectSmallArmRotation() {
        return sliderDirectSmallArm.getValue();
    }

    public float getDirectMainArmRotation() {
        return sliderDirectMainArm.getValue();
    }

    public float getDirectGripperRotation() {
        return sliderDirectGripper.getValue();
    }

    public float getRotation() {
        return 0.0f;
    }

    public float getPositionDistance() {
        return 0.0f;
    }

    public float getPositionHeight() {
        return 0.0f;
    }


    private JTextPane textDebug;
    private JTabbedPane tabbedPane;
    private JSlider sliderDirectRotation;
    private JSlider sliderDirectMainArm;
    private JSlider sliderDirectSmallArm;
    public JPanel panel;
    private  JSlider sliderDirectGripper;
    private JScrollPane debugScrollPane;
    public JLabel labelStickX;
    public JLabel labelStickY;
}
