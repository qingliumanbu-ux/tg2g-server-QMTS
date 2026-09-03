/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:       
Version:     1.0
Date:        2023-02-26
Description: 删除
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 删除制造标准
/// <para>
/// 获取输入参数： 
/// </para>
/// <para>数据库表：
/// 前台各个    画面的F5(删除)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

// service入口
BM2F_ENTERACE(qmts6c_del)

int f_qmts6c_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int flag = 0;

	/* 实体类定义 */
	CModel tqmts6c("TQMTS6C");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_del(conn);

	try
	{
		int count = bcls_rec->Tables[0].Rows.get_Count();

		for (int i = 0; i < count; i++)
		{
			tqmts6c.Reset();
			tqmts6c.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			/*tqmts6c["VALID_FLAG"] = "1";
			tqmts6c["DU_FLAG"] = "1";
			tqmts6c["DU_MAKER"] = s.userid;
			tqmts6c["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");*/
			//tqmtma8["VERSION"] = tqmtma8["VERSION"].ToDecimal() + 1;

			//修改成分标准内容
			//ST_NO, SG_SIGN1, SG_SIGN2, SG_SIGN3, SG_SIGN4, SG_SIGN5, SG_SIGN6, SG_SIGN7, SG_SIGN8, SG_SIGN9, VALID_FLAG, REMARK, REC_CREATOR, REC_CREATE_TIME, REC_REVISOR, REC_REVISE_TIME, ARCHIVE_FLAG, DU_FLAG, DU_MAKER, DU_TIME, VERSION, ARCHIVE_STAMP_NO, COMPANY_CODE, COMPANY_NAME,
			//	BASE_CODE, BASE_NAME, FACTORY_DIV, DO_DECI, DEST, PROD_CLASS_CODE, PROD_SERIES, P_PRD, INT_REQ, ST_NO_PRO,
			//tqmts6c.Update("VALID_FLAG, DU_FLAG, DU_MAKER, DU_TIME");

			tqmts6c.Delete();
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "","error=[{0}]", (const char*)str );

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
