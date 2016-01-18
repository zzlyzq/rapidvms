#include "config/viditem/vscviditemstorconf.h"
#include "config/viditem/vscviditemclientconf.h"
#include "config/viditem/vscviditemaddcam.h"
#include "config/viditem/vscviditemcamconf.h"
#include "config/viditem/vscviditemdiskconf.h"

VSCVidItemStorConf::VSCVidItemStorConf(VidStor cStor, ClientFactory &pFactory, QTreeWidgetItem *parent)
: VSCVidItemInf(pFactory, parent), m_cStor(cStor), m_pItemAddCam(NULL)
{
	QIcon icon1;
	icon1.addFile(QStringLiteral(":/device/resources/stor.png"), QSize(), QIcon::Normal, QIcon::Off);
	
	setIcon(0, icon1);
	setText(0, QApplication::translate(" ", m_cStor.strname().c_str(), 0));
	this->setTextColor(0, QColor(41, 42, 43));
	
	//TreeUpdated();

	m_pFactory.GetStorFactory().RegChangeNotify((void *)this, VSCVidItemStorConf::CallChange);

	UpdateOnline(m_pFactory.GetStorFactory().GetOnline(m_cStor.strid()));
	if (m_pFactory.GetStorFactory().GetOnline(m_cStor.strid()) == true)
	{
		TreeUpdated();
	}
	
	
}
VSCVidItemStorConf::~VSCVidItemStorConf()
{
	
}

bool VSCVidItemStorConf::CallChange(void* pParam, StorFactoryChangeData data)
{
    int dummy = errno;
    VSCVidItemStorConf * pObject = (VSCVidItemStorConf * )pParam;

    if (pObject)
    {
        return pObject->CallChange1(data);
    }
}
bool VSCVidItemStorConf::CallChange1(StorFactoryChangeData data)
{

	if (data.cId.strstorid() != m_cStor.strid())
	{
		return true;
	}

	switch (data.type)
	{
		case STOR_FACTORY_CAMERA_ADD:
		{
			CameraAdd(data.cCam);
			break;
		}
		case STOR_FACTORY_CAMERA_DEL:
		{
			CameraDelete(data.cId.strcameraid());
			break;
		}
		case STOR_FACTORY_CAMERA_ONLINE:
		{
			CameraOnline(data.cId.strcameraid());
			break;
		}
		case STOR_FACTORY_CAMERA_OFFLINE:
		{
			CameraOffline(data.cId.strcameraid());
			break;
		}
		case STOR_FACTORY_CAMERA_REC_ON:
		{
			CameraRecOn(data.cId.strcameraid());
			break;
		}
		case STOR_FACTORY_CAMERA_REC_OFF:
		{
			CameraRecOff(data.cId.strcameraid());
			break;
		}
		default:
		   	break;
	};

	return true;
}

void VSCVidItemStorConf::CameraAdd(VidCamera cCam)
{
	if (m_pItemAddCam == NULL)
	{
		return;
	}
	int cnt = m_pItemAddCam->childCount();

	for (int i = 0; i < cnt; i ++)
	{
		QTreeWidgetItem * pChild = m_pItemAddCam->child(i);
		VSCVidItemInf *pItem = dynamic_cast<VSCVidItemInf*>(pChild);
		if (pItem && pItem->GetId() == cCam.strid())
		{
			/* already in the list */
			m_pItemAddCam->removeChild(pChild);
		}
	}

	VSCVidItemCamConf *pItemCam = new  VSCVidItemCamConf(m_cStor, cCam,
								m_pFactory, m_pItemAddCam);

}
void VSCVidItemStorConf::CameraDelete(astring strId)
{
	if (m_pItemAddCam == NULL)
	{
		return;
	}
	int cnt = m_pItemAddCam->childCount();

	for (int i = 0; i < cnt; i ++)
	{
		QTreeWidgetItem * pChild = m_pItemAddCam->child(i);
		VSCVidItemInf *pItem = dynamic_cast<VSCVidItemInf*>(pChild);
		if (pItem && pItem->GetId() == strId)
		{
			/* already in the list */
			m_pItemAddCam->removeChild(pChild);
		}
	}
}
void VSCVidItemStorConf::CameraOnline(astring strId)
{
	if (m_pItemAddCam == NULL)
	{
		return;
	}
	int cnt = m_pItemAddCam->childCount();

	for (int i = 0; i < cnt; i ++)
	{
		QTreeWidgetItem * pChild = m_pItemAddCam->child(i);
		VSCVidItemInf *pItem = dynamic_cast<VSCVidItemInf*>(pChild);
		if (pItem && pItem->GetId() == strId)
		{
			/* already in the list */
			pItem->UpdateOnline(true);
		}
	}
}
void VSCVidItemStorConf::CameraOffline(astring strId)
{
	if (m_pItemAddCam == NULL)
	{
		return;
	}
	int cnt = m_pItemAddCam->childCount();

	for (int i = 0; i < cnt; i ++)
	{
		QTreeWidgetItem * pChild = m_pItemAddCam->child(i);
		VSCVidItemInf *pItem = dynamic_cast<VSCVidItemInf*>(pChild);
		if (pItem && pItem->GetId() == strId)
		{
			/* already in the list */
			pItem->UpdateOnline(false);
		}
	}
}

void VSCVidItemStorConf::CameraRecOn(astring strId)
{
}
void VSCVidItemStorConf::CameraRecOff(astring strId)
{
}

void VSCVidItemStorConf::TreeUpdated()
{
	qDeleteAll(takeChildren());
	/* Add fixed items */
	m_pItemAddCam = new VSCVidItemAddCam(m_cStor, m_pFactory, this);
	VSCVidItemDiskConf *pDiskConf = new VSCVidItemDiskConf(m_cStor, m_pFactory, this);
	
	VidCameraList cCamList = m_pFactory.GetStorFactory().GetVidCameraList(m_cStor.strid());
	StorClientOnlineMap bOnline = m_pFactory.GetStorFactory().GetVidCameraOnlineList(m_cStor.strid());

	int camSize = cCamList.cvidcamera_size();
	
	for (s32 i = 0; i < cCamList.cvidcamera_size(); i ++)
	{
		VidCamera pCam = cCamList.cvidcamera(i);
		/* All the camera are the child of the Add Cam */
		VSCVidItemCamConf *pItemCam = new  VSCVidItemCamConf(m_cStor, pCam, m_pFactory, m_pItemAddCam);
		StorClientOnlineMap::iterator it = bOnline.find(pCam.strid()), ite = bOnline.end();
		if (it != ite)
		{
			pItemCam->UpdateOnline(bOnline[pCam.strid()]);
		}
	}
	
	return;

}