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
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)/flag(删除标记——0:制造标准;1:成分标准)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表) 
/// 				TQMTS02(工序成分标准表)
/// 				TQMTS01(工序制造标准表)
/// 前台各个QMTS0x画面的F5(删除)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmtsa8_del)


int f_qmtsa8_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int flag = 0;

	/* 实体类定义 */
	CModel tqmtma8("TQMTMA8");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_del(conn);

	try
	{
		int count = bcls_rec->Tables[0].Rows.get_Count();

		for (int i = 0; i < count; i++)
		{
			tqmtma8.Reset();
			tqmtma8.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tqmtma8["VALID_FLAG"] = "1";
			tqmtma8["DU_FLAG"] = "1";
			tqmtma8["DU_MAKER"] = s.userid;
			tqmtma8["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			//tqmtma8["VERSION"] = tqmtma8["VERSION"].ToDecimal() + 1;

			//修改成分标准内容
			//ST_NO, SG_SIGN1, SG_SIGN2, SG_SIGN3, SG_SIGN4, SG_SIGN5, SG_SIGN6, SG_SIGN7, SG_SIGN8, SG_SIGN9, VALID_FLAG, REMARK, REC_CREATOR, REC_CREATE_TIME, REC_REVISOR, REC_REVISE_TIME, ARCHIVE_FLAG, DU_FLAG, DU_MAKER, DU_TIME, VERSION, ARCHIVE_STAMP_NO, COMPANY_CODE, COMPANY_NAME,
			//	BASE_CODE, BASE_NAME, FACTORY_DIV, DO_DECI, DEST, PROD_CLASS_CODE, PROD_SERIES, P_PRD, INT_REQ, ST_NO_PRO,
			tqmtma8.Update("VALID_FLAG, DU_FLAG, DU_MAKER, DU_TIME");
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
