using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using static Livestitch;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Button;

namespace livestitch
{
    public partial class Form1 : Form
    {
        private Toupcam cam_;
        private Bitmap bmp1_;
        private Bitmap bmp2_;
        private Livestitch stitch_;
        private bool bstitch_;
        private int crop_;
        private static Livestitch.IMAGEPRO_MALLOC ipmalloc;
        private void OnCamError(string str)
        {
            cam_?.Close();
            cam_ = null;
            MessageBox.Show(str);
        }

        private void OnEventImage()
        {
            if (bmp1_ != null)
            {
                Toupcam.FrameInfoV4 info = new Toupcam.FrameInfoV4();
                try
                {
                    BitmapData bmpdata1 = bmp1_.LockBits(new Rectangle(0, 0, bmp1_.Width, bmp1_.Height), ImageLockMode.WriteOnly, bmp1_.PixelFormat);
                    try
                    {
                        stitch_?.Pull(cam_.Handle, Convert.ToInt32(bstitch_), bmpdata1.Scan0, 24, 0, out info); // check the return value
                    }
                    finally
                    {
                        bmp1_.UnlockBits(bmpdata1);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
                pictureBox1.Image = bmp1_;
            }
        }

        public Form1()
        {
            InitializeComponent();
            bstitch_ = false;
            crop_ = 1;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            checkBox1.Enabled = false;
            checkBox2.Enabled = false;
        }

        private void OnStart(object sender, EventArgs e)
        {
            if (cam_ != null)
            {
                cam_?.Close();
                cam_ = null;
                stitch_?.Close();
                stitch_ = null;
                bstitch_ = false;
                timer1.Stop();
                checkBox1.Enabled = false;
                checkBox2.Enabled = false;
                button2.Enabled = false;
                button1.Text = "open";
                button2.Text = "Start Stitch";
            }
            else
            {
                Toupcam.DeviceV2[] arr = Toupcam.EnumV2();
                if (arr.Length <= 0)
                    MessageBox.Show("No camera found.");
                else
                {
                    if (arr.Length > 0)
                        startDevice(arr[0].id);
                    checkBox1.Enabled = true;
                    checkBox1.Checked = true;
                    checkBox2.Enabled = true;
                    checkBox2.Checked = true;
                    button2.Enabled = true;
                    timer1.Start();
                    button1.Text = "close";
                }
            }
        }

        private void OnStitch(object sender, EventArgs e)
        {
            if (bstitch_)
            {
                button2.Text = "Start Stitch";
                bstitch_ = false;
                checkBox1.Enabled = true;
                checkBox2.Enabled = true;
                stitch_?.Stop(1, crop_);
            }
            else
            {
                button2.Text = "Stop Stitch";
                bstitch_ =  true;
                checkBox1.Checked = false;
                checkBox1.Enabled = false;
                checkBox2.Enabled = false;
                stitch_?.Start();
            }
        }

        private void startDevice(string camId)
        {
            cam_ = Toupcam.Open(camId);
            if (cam_ != null)
            {
                ipmalloc = new IMAGEPRO_MALLOC(
                    (IntPtr size) =>
                    {
                        return Marshal.AllocHGlobal(size);
                    }
                    );
                Livestitch.init(ipmalloc);
                int width = 0, height = 0;
                if (cam_.get_Size(out width, out height))
                {
                    bmp1_ = new Bitmap(width, height, PixelFormat.Format24bppRgb);
                    cam_.StartPullModeWithCallback(
                        (Toupcam.eEVENT evt) =>
                        {
                            BeginInvoke((Action)(() =>
                            {
                                switch (evt)
                                {
                                    case Toupcam.eEVENT.EVENT_IMAGE:
                                        OnEventImage();
                                        break;
                                    case Toupcam.eEVENT.EVENT_ERROR:
                                        OnCamError("Generic Error");
                                        break;
                                    case Toupcam.eEVENT.EVENT_DISCONNECTED:
                                        OnCamError("Camera disconnect");
                                        break;
                                }
                            }));
                        }
                        );
                    stitch_ = Livestitch.New(Livestitch.eFormat.eRGB24, 1, width, height, 0,
                        (IntPtr outData, int stride, int outW, int outH, int curW, int curH, int curType,
                                        int posX, int posY, eQuality quality, float sharpness, int bUpdate, int bSize) =>
                        {
                            BeginInvoke((Action)(() =>
                            {
                                switch (quality)
                                {
                                    case eQuality.eZERO:
                                        label2.Text = "state is zero";
                                        break;
                                    case eQuality.eGOOD:
                                        label2.Text = "state is good";
                                        if (bmp2_ == null)
                                            bmp2_ = new Bitmap(outW, outH, PixelFormat.Format24bppRgb);
                                        if (bmp2_ != null)
                                        {
                                            Toupcam.FrameInfoV4 info = new Toupcam.FrameInfoV4();
                                            try
                                            {
                                                BitmapData bmpdata2 = bmp2_.LockBits(new Rectangle(0, 0, bmp2_.Width, bmp2_.Height), ImageLockMode.WriteOnly, bmp2_.PixelFormat);
                                                try
                                                {
                                                    stitch_.ReadData(bmpdata2.Scan0, outW, outH, 0, 0, 0, 0);
                                                }
                                                finally
                                                {
                                                    bmp2_.UnlockBits(bmpdata2);
                                                }
                                            }
                                            catch (Exception ex)
                                            {
                                                MessageBox.Show(ex.ToString());
                                            }
                                            pictureBox2.Image = bmp2_;
                                        }
                                        break;
                                    case eQuality.eCAUTION:
                                        label2.Text = "state is caution";
                                        break;
                                    case eQuality.eBAD:
                                        label2.Text = "state is bad";
                                        break;
                                    case eQuality.eWARNING:
                                        label2.Text = "state is warning";
                                        break;
                                }
                            }));
                        },
                        (Livestitch.eEvent evt) =>
                        {
                            BeginInvoke((Action)(() =>
                            {
                                /* this run in the UI thread */
                                switch (evt)
                                {
                                    case Livestitch.eEvent.eERROR:
                                        MessageBox.Show("stitch generic error");
                                        break;
                                    case Livestitch.eEvent.eNOMEM:
                                        MessageBox.Show("stitch out of memory");
                                        break;
                                    default:
                                        break;
                                }
                            }));
                        });
                    if (stitch_ == null)
                        MessageBox.Show("Failed to new stitch");
                    else 
                        GC.KeepAlive(stitch_);
                }

                timer1.Start();
            }
        }

        private void OnClosing(object sender, FormClosingEventArgs e)
        {
            if (bstitch_)
                stitch_?.Stop(1, crop_);
            stitch_ = null;
            cam_?.Close();
            cam_ = null;
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            cam_?.put_AutoExpoEnable(checkBox1.Checked);
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            crop_ = Convert.ToInt32(checkBox2.Checked);
        }

        private void OnTimer1(object sender, EventArgs e)
        {
            if (cam_ != null)
            {
                uint nFrame = 0, nTime = 0, nTotalFrame = 0;
                if (cam_.get_FrameRate(out nFrame, out nTime, out nTotalFrame) && (nTime > 0))
                    label1.Text = string.Format("{0}; fps = {1:#.0}", nTotalFrame, ((double)nFrame) * 1000.0 / (double)nTime);
            }
        }
    }
}