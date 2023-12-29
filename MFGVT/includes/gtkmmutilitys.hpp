#ifndef __GTKMMUTILITYS_HPP__
#define __GTKMMUTILITYS_HPP__

#include <string>
#include <thread>

#include <gtkmm-3.0/gtkmm/hvbox.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/switch.h>
#include <gtkmm-3.0/gtkmm/dialog.h>
#include <glibmm-2.4/glibmm/dispatcher.h>


/// @brief cree un bouton de type switch avec sont label
class SwitchUser : public Gtk::HBox
{
   public :
      SwitchUser( bool _default = false , std::string const & _label ="");


      void set_label(std::string const & _label);
      bool get_active(void);
      void set_active(bool _stat);

     
       Glib::SignalProxyProperty  signal_changed(void);

   private:
      Gtk::Switch m_switchButton;
      Gtk::Label m_label;

} ;

/// @brief initialisse le bouton
/// @param _default etat par defaut
/// @param _label nom du bouton
SwitchUser::SwitchUser(bool _default  , std::string const & _label )
{
    this->pack_start( this->m_label , Gtk::PACK_SHRINK);
    this->pack_start( this->m_switchButton , Gtk::PACK_SHRINK );

    this->set_label(_label);
    this->set_active(_default);
}


/// @brief met a jour le nom du bouton
/// @param _label 
void SwitchUser::set_label(std::string const & _label)
{
    this->m_label.set_label(_label + " ");
}

/// @brief renvoie l'etat du bouton
bool SwitchUser::get_active(void)
{
    return this->m_switchButton.get_active();
}

/// @brief impose un etat au bouton
/// @param _stat etat a imposer
void SwitchUser::set_active(bool _stat)
{
    this->m_switchButton.set_active(_stat); 
    this->m_switchButton.property_active().signal_changed();
}

/// @brief renvoie le signal associer au changement d'etat
/// @return 
Glib::SignalProxyProperty  SwitchUser::signal_changed(void)
{
    return  this->m_switchButton.property_active().signal_changed();
}



/// @brief cree un dialog dans un thread séparé
class DialogThread : public Gtk::Dialog
{
   public :
      DialogThread(void);
      ~DialogThread(void);

      void run(void);
      void stop(void);

   private :

      bool thread_func(void);

       std::thread m_th;
       std::atomic<bool> m_continue , m_end ;
       Glib::Dispatcher m_signalDone;
      
};

/// @brief initialisation de la fenetre 
/// @param  
DialogThread::DialogThread(void):Gtk::Dialog()
{
    this->m_signalDone.connect(sigc::mem_fun(this, &DialogThread::show_all));
    this->m_continue = false;
    this->m_end = false;
}

/// @brief arret de la fenetre et de son thread
/// @param  
DialogThread::~DialogThread(void)
{
    this->stop();
}

/// @brief demare le thread
/// @param  
void DialogThread::run(void)
{
    
    if(this->m_continue || this->m_th.joinable())
        return ;

    this->m_end = false;
    this->m_continue = true;
    this->m_th =  std::thread(&DialogThread::thread_func, this);
}

/// @brief arret du thread
/// @param  
void DialogThread::stop(void)
{
    if(!this->m_continue)
        return;
    
    this->m_continue = false;
    if(this->m_th.joinable())
        this->m_th.join();

}

/// @brief routine du thread
/// @param  
/// @return 
bool DialogThread::thread_func(void)
{
    while( this->m_continue )
    {
        this->m_signalDone.emit();

        //emet un delai avant la mise a jour de la fenetre
        const std::chrono::duration<double, std::milli> duration(500);

        //s'assure que apres le delais on est tjr ouvert
        if(!this->m_continue)
            break;

        std::this_thread::sleep_for(duration);
    }

    this->m_end = true;
    return true;
}

#endif