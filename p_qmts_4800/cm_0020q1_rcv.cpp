/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   yiling
Version:    1.0
Date:     2016-04-11
Description: 工艺卡制造标准接收
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 工艺卡制造标准接收
/// <para>
/// 1.工艺卡制造标准接收
/// </para>
/// <para>数据库表：       </para>
/// <para>主调用函数：     </para>
/// </summary>
/// <returns>无</returns>
===========================================================</remark>*/

//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"
#include "tqmts04.h"
#include "tqmts08.h"
#include "tqmts0a.h"
#include "tqmts0l.h"
#include "tqmts0m.h"
#include "tqmts07.h"
#include "tqmts05p.h"
#include "tqmts06.h"
#include "tqmts03.h"
#include "tqmts05.h"
#include "tqmts0x.h"

BM2F_ENTERACE_TELE(cm_0020q1_rcv)
int f_cm_0020q1_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	// 程序用变量

	CString  v_flag = " ";
	int   doFlag = 0;
	CString lpsz_tc_no = " ";
	CString s_table_name = "";
	CString s_st_no = " ";
	CString s_factory_div = " ";
	/* ***** 创建电文处理对象 ***** */
	EPEX epex(&s, conn);
	/* 使用的表结构变量 */
	CTQMTS04 tqmts04(conn);
	CTQMTS08 tqmts08(conn);
	CTQMTS0A tqmts0a(conn);
	CTQMTS0L tqmts0l(conn);
	CTQMTS0M tqmts0m(conn);
	CTQMTS07 tqmts07(conn);
	CTQMTS05P tqmts05p(conn);
	CTQMTS06 tqmts06(conn);
	CTQMTS03 tqmts03(conn);
	CTQMTS05 tqmts05(conn);
	CTQMTS0X tqmts0x(conn);

	CString sqlstr;
	CDbCommand  cmd_inq(conn);



	try
	{
		tqmts04.Reset();
		tqmts08.Reset();
		tqmts0a.Reset();
		tqmts0l.Reset();
		tqmts0m.Reset();
		tqmts07.Reset();
		tqmts05p.Reset();
		tqmts06.Reset();
		tqmts03.Reset();
		tqmts05.Reset();
		tqmts0x.Reset();
		s_table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim().ToLower();
		s_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		s_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();

		Log::Info("", __FUNCTION__, "s_table_name=[{0}]s_st_no[{1}]s_factory_div[{2}]", s_table_name, s_st_no, s_factory_div);
		tqmts0x.ST_NO = s_st_no;
		tqmts0x.FACTORY_DIV = s_factory_div;
		tqmts0x.Query("ST_NO,FACTORY_DIV");
		Log::Info("", __FUNCTION__, "tqmts0x.VALID_FLAG=[{0}]s_st_no[{1}]", tqmts0x.VALID_FLAG, s_st_no);

		if (tqmts0x.VALID_FLAG.Trim() == "1") ////生效
		{
			if (tqmts0x.IDX_NO_01.Trim() == "")  /////索引减少时，删除制造标准
			{
				tqmts04.ST_NO = s_st_no;
				tqmts04.FACTORY_DIV = s_factory_div;
				tqmts04.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_02.Trim() == "")
			{
				tqmts08.ST_NO = s_st_no;
				tqmts08.FACTORY_DIV = s_factory_div;
				tqmts08.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_03.Trim() == "")
			{
				tqmts0a.ST_NO = s_st_no;
				tqmts0a.FACTORY_DIV = s_factory_div;
				tqmts0a.Delete("ST_NO,FACTORY_DIV");
			}	
			if (tqmts0x.IDX_NO_04.Trim() == "")
			{
				tqmts0l.ST_NO = s_st_no;
				tqmts0l.FACTORY_DIV = s_factory_div;
				tqmts0l.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_05.Trim() == "")
			{
				tqmts0m.ST_NO = s_st_no;
				tqmts0m.FACTORY_DIV = s_factory_div;
				tqmts0m.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_06.Trim() == "")
			{
				tqmts07.ST_NO = s_st_no;
				tqmts07.FACTORY_DIV = s_factory_div;
				tqmts07.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_07.Trim() == "")
			{
				tqmts05p.ST_NO = s_st_no;
				tqmts05p.FACTORY_DIV = s_factory_div;
				tqmts05p.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_08.Trim() == "")
			{
				tqmts06.ST_NO = s_st_no;
				tqmts06.FACTORY_DIV = s_factory_div;
				tqmts06.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_09.Trim() == "")
			{
				tqmts03.ST_NO = s_st_no;
				tqmts03.FACTORY_DIV = s_factory_div;
				tqmts03.Delete("ST_NO,FACTORY_DIV");
			}
			if (tqmts0x.IDX_NO_10.Trim() == "")
			{
				tqmts05.ST_NO = s_st_no;
				tqmts05.FACTORY_DIV = s_factory_div;
				tqmts05.Delete("ST_NO,FACTORY_DIV");
			}

			if (s_table_name == "tqmts04")
			{
				tqmts04.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts04.Delete("ST_NO,FACTORY_DIV");
				tqmts04.Insert();
			}
			if (s_table_name == "tqmts08")
			{
				tqmts08.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts08.Delete("ST_NO,FACTORY_DIV");
				tqmts08.Insert();
			}
			if (s_table_name == "tqmts0a")
			{
				tqmts0a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts0a.Delete("ST_NO,FACTORY_DIV");
				tqmts0a.Insert();
			}
			if (s_table_name == "tqmts0l")
			{
				tqmts0l.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts0l.Delete("ST_NO,FACTORY_DIV");
				tqmts0l.Insert();
			}
			if (s_table_name == "tqmts0m")
			{
				tqmts0m.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts0m.Delete("ST_NO,FACTORY_DIV");
				tqmts0m.Insert();
			}
			if (s_table_name == "tqmts07")
			{
				tqmts07.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts07.Delete("ST_NO,FACTORY_DIV");
				tqmts07.Insert();
			}
			if (s_table_name == "tqmts05p")
			{
				tqmts05p.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts05p.Delete("ST_NO,FACTORY_DIV");
				tqmts05p.Insert();
			}
			if (s_table_name == "tqmts06")
			{
				tqmts06.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts06.Delete("ST_NO,FACTORY_DIV");
				tqmts06.Insert();
			}
			if (s_table_name == "tqmts03")
			{
				tqmts03.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts03.Delete("ST_NO,FACTORY_DIV");
				tqmts03.Insert();
			}
			if (s_table_name == "tqmts05")
			{
				tqmts05.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tqmts05.Delete("ST_NO,FACTORY_DIV");
				tqmts05.Insert();
			}

		}
		else if (tqmts0x.VALID_FLAG.Trim() == "0")/////失效
		{
			////制造标准不做处理
		}
		else ///删除
		{
			tqmts04.ST_NO = s_st_no;
			tqmts08.ST_NO = s_st_no;
			tqmts0a.ST_NO = s_st_no;
			tqmts0l.ST_NO = s_st_no;
			tqmts0m.ST_NO = s_st_no;
			tqmts07.ST_NO = s_st_no;
			tqmts05p.ST_NO = s_st_no;
			tqmts06.ST_NO = s_st_no;
			tqmts03.ST_NO = s_st_no;
			tqmts05.ST_NO = s_st_no;

			tqmts04.FACTORY_DIV = s_factory_div;
			tqmts08.FACTORY_DIV = s_factory_div;
			tqmts0a.FACTORY_DIV = s_factory_div;
			tqmts0l.FACTORY_DIV = s_factory_div;
			tqmts0m.FACTORY_DIV = s_factory_div;
			tqmts07.FACTORY_DIV = s_factory_div;
			tqmts05p.FACTORY_DIV = s_factory_div;
			tqmts06.FACTORY_DIV = s_factory_div;
			tqmts03.FACTORY_DIV = s_factory_div;
			tqmts05.FACTORY_DIV = s_factory_div;

			if (s_table_name == "tqmts04")
			{
				tqmts04.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts08")
			{
				tqmts08.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts0a")
			{
				tqmts0a.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts0l")
			{
				tqmts0l.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts0m")
			{
				tqmts0m.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts07")
			{
				tqmts07.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts05p")
			{
				tqmts05p.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts06")
			{
				tqmts06.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts03")
			{
				tqmts03.Delete("ST_NO,FACTORY_DIV");
			}
			if (s_table_name == "tqmts05")
			{
				tqmts05.Delete("ST_NO,FACTORY_DIV");
			}
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EITrace对象的sys_Trace.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
