#include "MantidAPI/RefAxis.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidKernel/Exception.h"

DECLARE_WORKSPACE(EventWorkspace)

namespace Mantid
{

namespace DataObjects
{
  using Kernel::Exception::NotImplementedError;

  // get a reference to the logger
  Kernel::Logger& EventWorkspace::g_log
                 = Kernel::Logger::get("EventWorkspace");

  //---- Constructors -------------------------------------------------------------------
  EventWorkspace::EventWorkspace()
  {
  }
  EventWorkspace::~EventWorkspace()
  {}


  void EventWorkspace::init(const int &NVectors, const int &XLength,
          const int &YLength)
  {
    // Check validity of arguments
    if (NVectors <= 0)
    {
      g_log.error("Negative or 0 Number of Pixels specified to EventWorkspace::init");
      throw std::out_of_range("Negative or 0 Number of Pixels specified to EventWorkspace::init");
    }

    m_noVectors = NVectors;
    data.resize(m_noVectors, NULL);
  }



  //-----------------------------------------------------------------------------
  int EventWorkspace::size() const
  {
    return this->data.size() * this->blocksize();
  }

  //-----------------------------------------------------------------------------
  int EventWorkspace::blocksize() const
  {
    // Pick the first pixel to find the blocksize.
    EventListVector::iterator it = data.begin();
    if (it == data.end())
    {
      throw std::range_error("EventWorkspace::blocksize, no pixels in workspace, therefore cannot determine blocksize (# of bins).");
    }
    else
    {
      return (*it)->histogram_size();
    }
  }

  //-----------------------------------------------------------------------------
  const int EventWorkspace::getNumberHistograms() const
  {
    return this->data.size();
  }

  //-----------------------------------------------------------------------------
  size_t EventWorkspace::getNumberEvents() const
  {
    size_t total = 0;
    for (EventListVector::const_iterator it = this->data.begin();
        it != this->data.end(); it++) {
      total += (*it)->getNumberEvents();
    }
    return total;
  }

  //-----------------------------------------------------------------------------
  const bool EventWorkspace::isHistogramData() const
  {
    return true;
  }


  //-----------------------------------------------------------------------------
  // --- Data Access ----
  //-----------------------------------------------------------------------------

  EventList& EventWorkspace::getEventList(const int pixelid)
  {
    //An empty entry will be made if needed
    EventListMap::iterator it = this->data_map.find(pixelid);
    if (it == this->data_map.end())
    {
      //Need to make a new one!
      EventList * newel = new EventList();
      //Save it in the map
      this->data_map[pixelid] = newel;
      return (*newel);
    }
    else
    {
      //Already exists; return it
      return *this->data_map[pixelid];
    }
  }

  EventList& EventWorkspace::getEventListAtWorkspaceIndex(const int workspace_index)
  {
    //An empty entry will be made if needed
    return *this->data[workspace_index];
  }



  //-----------------------------------------------------------------------------
  void EventWorkspace::doneLoadingData()
  {
    //Ok, we need to take the data_map, and turn it into a data[] vector.

    //Let's make the vector big enough.
    if (this->data_map.size() > m_noVectors)
    {
      //Too many vectors! Why did you initialize it bigger than you needed to, silly?
      for (int i=this->data_map.size(); i<m_noVectors; i++)
        //Delete the offending EventList so as to avoid memory leaks.
        delete this->data[i];
    }
    //Now resize
    m_noVectors = this->data_map.size();
    this->data.resize(m_noVectors, NULL);

    //For the mapping workspace
    int* index_table = new int [m_noVectors];
    int* pixelid_table = new int [m_noVectors];

    int counter = 0;
    EventListMap::iterator it;
    for (it = this->data_map.begin(); it != this->data_map.end(); it++)
    {
      //Iterate through the map
      index_table[counter] = counter;
      pixelid_table[counter] = it->first; //The key = the pixelid

      //Copy the pointer to the event list in there.
      this->data[counter] = it->second;

      //std::cout << "added" << std::endl;
      counter++;
    }

    //Save the mapping
    mutableSpectraMap().populate(index_table, pixelid_table, m_noVectors);

    //Now clear the data_map
    this->data_map.clear();

    //Get your memory back :)
    delete [] index_table;
    delete [] pixelid_table;
  }


  //-----------------------------------------------------------------------------
  // Note: these non-const access methods will throw NotImplementedError
  MantidVec& EventWorkspace::dataX(const int index)
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::dataX, histogram number out of range");
    return this->data[index]->dataX();
  }

  MantidVec& EventWorkspace::dataY(const int index)
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::dataY, histogram number out of range");
    return this->data[index]->dataY();
  }

  MantidVec& EventWorkspace::dataE(const int index)
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::dataE, histogram number out of range");
    return this->data[index]->dataE();
  }


  //-----------------------------------------------------------------------------
  // --- Const Data Access ----
  //-----------------------------------------------------------------------------
  //Can't use the [] operator for const access; you need to use find, which returns an iterator, that returns a struct with 2 members.

  const MantidVec& EventWorkspace::dataX(const int index) const
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::dataX, histogram number out of range");
    return this->data[index]->dataX();

  }

  const MantidVec& EventWorkspace::dataY(const int index) const
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::dataY, histogram number out of range");
    return this->data[index]->dataY();
  }

  const MantidVec& EventWorkspace::dataE(const int index) const
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::dataE, histogram number out of range");
    return this->data[index]->dataE();
  }

  Kernel::cow_ptr<MantidVec> EventWorkspace::refX(const int index) const
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::refX, histogram number out of range");
    return this->data[index]->getRefX();

  }

  //-----------------------------------------------------------------------------
  // --- Histogramming ----
  //-----------------------------------------------------------------------------
  void EventWorkspace::setX(const int index,
      const Kernel::cow_ptr<MantidVec> &x)
  {
    if ((index >= this->m_noVectors) || (index < 0))
      throw std::range_error("EventWorkspace::setX, histogram number out of range");
    this->data[index]->setX(x);
  }

  void EventWorkspace::setAllX(Kernel::cow_ptr<MantidVec> &x)
  {
    EventListVector::iterator i = this->data.begin();
    for( ; i != this->data.end(); ++i )
    {
      (*i)->setX(x);
    }
  }


  //-----------------------------------------------------------------------------
  // --- Frame Times ----
  //-----------------------------------------------------------------------------

  /** Get the absolute time corresponding to the give frame ID */
  ptime EventWorkspace::getTime(const size_t frameId)
  {
    if ((frameId < 0) || (frameId >= this->frameTime.size()))
      throw std::range_error("EventWorkspace::getTime called with a frameId outside the range.");

    //Will throw an exception if you are out of bounds.
    return this->frameTime.at(frameId);
  }

  /** Add ahe absolute time corresponding to the give frame ID */
  void EventWorkspace::addTime(const size_t frameId, ptime absoluteTime)
  {
    if (frameId < 0)
      throw std::range_error("EventWorkspace::addTime called with a frameId below 0.");
    //Resize, if needed, and fill with the default ptime (which is not-a-time)
    if (this->frameTime.size() <= frameId)
      this->frameTime.resize(frameId+1, ptime());
    this->frameTime[frameId] = absoluteTime;
  }


} // namespace DataObjects
} // namespace Mantid
