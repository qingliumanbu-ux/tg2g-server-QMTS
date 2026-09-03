/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2011 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************	
*  程序名称			: <%f_qmts_round%>
*  程序描述			: <%根据元素格式,对成分实际值进行四舍六入五单双的修约%>
*  备注说明			: 
*  修改历史			: 		
*  		<%2007-01-17%> 	<%孙羽田%>			(ADD)程序建立
*			... ...
* **************************************************************************** */

//框架公用头文件，勿删
#include "stdafx.h"
#include "math.h"
BM2_FUNCTION_EXPORT
 int f_qmts_round(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	//程序用头文件
	int doFlag = 0;
	int fetchRowCount;
	int i;
	int k;

	/*在SQL语句中使用的变量*/
	CDecimal d_elm_value = 0;
	CDecimal f_elm_value = 0;
	CDecimal g_elm_value = 0;
	CString   s_elm_format;
	int   seq= 0;
	CString format1;
	CString format2;
	int iformat1= 0;
	int iformat2= 0;
	int  value_zs= 0;
	CDecimal  value_xs= 0;
	CDecimal  r_elm_value= 0;
	int  value_re= 0;
	int  fin_value_re = 0;
	CDecimal ch_value=0;
	CString ch_value_zs;
	CString ch_value_xs;

	CDbCommand cmd_inq(conn);
	CString sqlstr;

	CTracer log(__FUNCTION__);
	try
	{
		/*获得输入参数*/
		d_elm_value = bcls_rec->Tables[0].Rows[0]["elm_value"];
		s_elm_format = bcls_rec->Tables[0].Rows[0]["elm_format"];
		//Log::Trace("", "","f_qmtc_ysgs_change----elm_value[{0}]elm_format[{1}]",d_elm_value.ToFloat(),(const char*)s_elm_format);

		//EPCutStrZ(s_elm_format,1,1,format1);/*截取获得元素格式的整数位*/
		//EPCutStrZ(s_elm_format,2,1,format2);/*截取获得元素格式的小数位*/
		//format1=s_elm_format.Substring(0,1);
		//format2=s_elm_format.Substring(1,1);
		//iformat1 = atoi(format1);/*截取需要的整数位*/
		//iformat2 = atoi(format2);/*截取需要的小数*/
		Log::Trace("", "", "d_elm_value[{0}]", d_elm_value);

		iformat2 = atoi(s_elm_format);/*截取需要的小数*/
		f_elm_value = d_elm_value * pow(10.0,iformat2);

		value_re=f_elm_value.Floor().ToInt32();			/*取整*/
		//以下是四舍六入五单双的修约
		//四舍六入五单双”的法则：即看要保留的有效数字后一位数字，如果大于5，向前进一位，小于4则舍去。等于5则看5后，如5后不全为0，则进一位。5后全为零则看5前，若为奇，则进1，若为偶，则舍去。
		g_elm_value = f_elm_value - value_re;
		Log::Trace("", "", "value_re[{0}]g_elm_value[{1}]", value_re, g_elm_value);
		if (g_elm_value == 0.5)
		{
			if (value_re % 2 == 0)
				fin_value_re = value_re;
			else
				fin_value_re = value_re+1;
		}
		else
			fin_value_re = (f_elm_value + 0.5).Floor().ToInt32();

		//Log::Trace("", "","f_elm_value[{0}]value_re[{1}]ch_value[{2}]",f_elm_value.ToDouble(),value_re,(const char*)ch_value);

		ch_value = fin_value_re/pow(10.0, iformat2);

		

		 
		if (!bcls_ret->Tables[0].Columns.Contains("ch_value"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "ch_value");
		}
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["ch_value"] = ch_value;

		//Log::Trace("", "","f_qmtc_ysgs_change----ch_value[{0}]",(const char*)ch_value);
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

 }