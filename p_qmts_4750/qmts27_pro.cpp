/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     wsl
Version:    1.0
Date:       2024-01-13
Description: QMTS27增删改
**************************************************/
//框架头文件
#include "stdafx.h" 
//业务头文件

//外部函数声明
int f_t82309_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送低倍实绩电文

BM2F_ENTERACE(qmts27_pro)

int f_qmts27_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");
	CString	c_datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_seq_no = 0;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		c_datetime = datetime.Substring(0, 8) + "000000";
		/* 获得传入参数 */

		if (bcls_rec->Tables.IndexOf("QMTS27_ADD") >= 0)
		{
			Log::Trace("", __FUNCTION__, "COUNT",'111');
			tqmts27.Reset();
			tqmts27.MergeFrom(bcls_rec->Tables["QMTS27_ADD"].Rows[0]);
			tqmts27.TrimOrBlank();
			if (tqmts27.QueryCount("MAT_NO") > 0)
			{
				tqmts27["REC_REVISOR"] = s.userid;
				tqmts27["REC_REVISE_TIME"] = datetime;
				tqmts27.TrimOrBlank();
				tqmts27.Update("REC_REVISOR,REC_REVISE_TIME,ARCHIVE_FLAG,ARCHIVE_STAMP_NO,COMPANY_CODE,COMPANY_NAME,HEAT_NO,ST_NO,MAT_NO,MAT_THICK,CENTER_SGRG_POROSITY,CRACK_CENTER,EQUIAXED_GRAIN_WIDTH,EQUIAXED_GRAIN_PERCENTAGE,TRI_CRACK_GRADE,ANGLE_CRACK_GRADE,TRANSVERSE_INTERNAL_CRACK,LONGITUDINAL_INTERNAL_CRACK,OTHER_DEFECTS_DESCRIPTION,CRACK,INNER_ARC_WIDTH,OUTTER_ARC_WIDTH,CENTRE_THICKNESS,EDGE_THICKNESS,TEST_PROCEDURE,REFERENCE_STANDARD,CHECK_MAKER,DEV_CODE,C_DIV,AREA,SAMPLE_MAKER,EQUIAXED_GRAIN_PERCENTAGE_R", "MAT_NO");

				//发送低倍实绩电文
				bcls_rec_s.Tables[0].Rows.Add();
				bcls_rec_s.Tables[0].Rows[0]["DEAL_FLAG"] = "U";
				bcls_rec_s.Tables[0].Rows[0]["HEAT_NO"] = tqmts27["HEAT_NO"];
				bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tqmts27["MAT_NO"];
				Log::Trace("", __FUNCTION__, "MAT_NO[{0}]  ", tqmts27["MAT_NO"].ToString());
				doFlag = f_t82309_snd(&bcls_rec_s, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				return 0;
			}
			///* 新增事件信息 */
			tqmts27["REC_CREATOR"] = s.userid;   //记录创建责任者
			tqmts27["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			tqmts27.TrimOrBlank();
			tqmts27.Insert();

			//发送低倍实绩电文
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["DEAL_FLAG"] = "I";
			bcls_rec_s.Tables[0].Rows[0]["HEAT_NO"] = tqmts27["HEAT_NO"];
			bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tqmts27["MAT_NO"];
			Log::Trace("", __FUNCTION__, "MAT_NO[{0}]  ", tqmts27["MAT_NO"].ToString());
			doFlag = f_t82309_snd(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("QMTS27_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["QMTS27_DEL"].Rows.get_Count(); i++)
			{
				tqmts27.Reset();
				tqmts27.MergeFrom(bcls_rec->Tables["QMTS27_DEL"].Rows[i]);
				tqmts27.TrimOrBlank();
				if (tqmts27["MAT_NO"].ToString().Trim() != "")
				{
					/* 删除事件信息 */
					tqmts27.Delete("MAT_NO");
				}
				//发送低倍实绩电文
				bcls_rec_s.Tables[0].Rows.Add();
				bcls_rec_s.Tables[0].Rows[0]["DEAL_FLAG"] = "D";
				bcls_rec_s.Tables[0].Rows[0]["HEAT_NO"] = tqmts27["HEAT_NO"];
				bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tqmts27["MAT_NO"];
				Log::Trace("", __FUNCTION__, "MAT_NO[{0}]  ", tqmts27["MAT_NO"].ToString());
				doFlag = f_t82309_snd(&bcls_rec_s, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


