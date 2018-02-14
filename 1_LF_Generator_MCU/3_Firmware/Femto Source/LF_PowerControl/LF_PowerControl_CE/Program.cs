using System;
using System.Linq;
using System.Collections.Generic;
using System.Windows.Forms;

namespace LF_PowerControl_CE
{
    static class Program
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [MTAThread]
        static void Main ()
        {
            Application.Run (new Form_Main ());
        }
    }
}